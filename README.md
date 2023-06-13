# OS_IHW4
# Габдуллина Алия Маратовна БПИ214
# Вариант 26. Условие задачи: 
Задача о производстве булавок. В цехе по заточке булавок все
необходимые операции осуществляются тремя рабочими. Первый
из них берет булавку и проверяет ее на предмет кривизны. Если
булавка не кривая, то рабочий передает ее своему напарнику. Иначе выбрасывает в отбраковку. Напарник осуществляет собственно
заточку и передает заточенную булавку третьему рабочему, который осуществляет контроль качества операции бракуя булавку или
отдавая на упаковку. Требуется создать приложение, моделирующее работу цеха. При решении использовать парадигму
«производитель-потребитель». Следует учесть, что каждая из операций выполняется за случайное время которое не связано с конкретным рабочим. Возможны различные способы реализации передачи (на усмотрение разработчика). Либо непосредственно по одной булавке, либо через ящики, в которых буферизируется некоторое конечное количество булавок.


Задание выполнено на 7

# Код на 4-5: 
Программа создает один родительский и три дочерних процесса, каждый из которых выполняет свою функцию:

0. Сервер - цех
1. Клиенты - рабочие, первый проверяет булавку на кривизну, второй затачивает булавку, третий контролирует качество булавки.

Логика работы программы: Запускается сервер, который будет слушать порт (8080) и ждать подключений от клиентов.
Запускаются клиенты, которые представляют рабочих. Каждый клиент будет подключаться к соответствующему серверу.
Клиенты проверяют булавки на кривизну и отправляют булавки на серверы для выполнения операций. Булавки передаются по сети с использованием сокетов. Далее серверы принимают информацию о булавках от клиентов и выводят сообщения не экран.

Код клиента:

```

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

int main() {
    int client_socket;
    struct sockaddr_in server_address;

    // Создание сокета клиента
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        perror("Ошибка при создании сокета клиента");
        exit(1);
    }

    // Настройка адреса сервера
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(PORT);

    printf("Подключение рабочего к серверу установлено\n");

    // Отправка данных серверу
    int r = rand() % 2;
    const char *message;
    if (r == 0) {
        message = "Булавка кривая";
    } else {
        message = "Булавка прямая";
    }
    sendto(client_socket, message, strlen(message), 0, (struct sockaddr *)&server_address, sizeof(server_address));

    // Чтение данных от сервера
    char buffer[256];
    socklen_t server_address_length = sizeof(server_address);
    ssize_t bytes_read = recvfrom(client_socket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&server_address, &server_address_length);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Получены данные от сервера: %s\n", buffer);
    }

    // Закрытие сокета клиента
    close(client_socket);

    return 0;
}
```

Код сервера: 
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

struct Pin {
    int id;
    int is_curved;
};

void handle_client(int client_socket, struct sockaddr_in client_address) {
    // Обработка клиентского соединения
    struct Pin pin;
    pin.id = 1;
    pin.is_curved = 0;
    sendto(client_socket, &pin, sizeof(struct Pin), 0, (struct sockaddr *)&client_address, sizeof(client_address));

    struct Pin received_pin;
    socklen_t client_address_length = sizeof(client_address);
    ssize_t bytes_read = recvfrom(client_socket, &received_pin, sizeof(struct Pin), 0, (struct sockaddr *)&client_address, &client_address_length);
    if (bytes_read == sizeof(struct Pin)) {
        // Обработка полученной булавки
        if (received_pin.is_curved) {
            printf("Булавка %d кривая. Отбракована.\n", received_pin.id);
        } else {
            printf("Булавка %d передана следующему рабочему.\n", received_pin.id);
            sendto(client_socket, &received_pin, sizeof(struct Pin), 0, (struct sockaddr *)&client_address, sizeof(client_address));
        }
    }
}

int main() {
    int server_socket;
    struct sockaddr_in server_address;

    // Создание сокета сервера
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        perror("Ошибка при создании сокета сервера");
        exit(1);
    }

    // Настройка адреса сервера
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Привязка сокета к адресу сервера
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Ошибка при привязке сокета к адресу сервера");
        exit(1);
    }

    printf("Сервер запущен. Ожидание подключений...\n");

    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);

    // Обработка подключений от клиентов
    while (1) {
        // Принятие подключения от клиента
        struct Pin received_pin;
        ssize_t bytes_read = recvfrom(server_socket, &received_pin, sizeof(struct Pin), 0, (struct sockaddr *)&client_address, &client_address_length);
        if (bytes_read == sizeof(struct Pin)) {
            printf("Получено подключение от клиента\n");

            // Создание отдельного процесса для обработки клиента
            if (fork() == 0) {
                handle_client(server_socket, client_address);
                exit(0);
            }
        }
    }

    // Закрытие сокета сервера
    close(server_socket);

    return 0;
}
```

#Код на 6-7:

Аналогично предыдущим пунктам, только добавляется наблюдатель. Теперь сервер отправляет сообщения и рабочим, и наблюдателю. 

Код клиента:
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

void update_system_status(const char *status) {
    printf("System Status: %s\n", status);
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;

    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &(server_address.sin_addr)) <= 0) {
        perror("Неверный адрес");
        exit(EXIT_FAILURE);
    }

    update_system_status("Подключение к серверу успешно");

    // Обработка запросов к серверу
    int r = rand() % 2;
    const char *message;
    if (r == 0) {
        message = "Булавка кривая";
    } else {
        message = "Булавка прямая";
    }
    sendto(client_socket, message, strlen(message), 0, (struct sockaddr *)&server_address, sizeof(server_address));

    // Чтение данных от сервера
    char buffer[256];
    socklen_t server_address_length = sizeof(server_address);
    ssize_t bytes_read = recvfrom(client_socket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&server_address, &server_address_length);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Получены данные от сервера: %s\n", buffer);
    }

    close(client_socket);

    return 0;
}
```

Код сервера:
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

void update_system_status(const char *status) {
    printf("System Status: %s\n", status);
}

int main() {
    int server_socket, observer_socket;
    struct sockaddr_in server_address, observer_address;

    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
        perror("Создание сокета не произошло");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    update_system_status("Сервер запущен");

    socklen_t observer_address_length = sizeof(observer_address);
    if (recvfrom(server_socket, NULL, 0, MSG_PEEK, (struct sockaddr *)&observer_address, &observer_address_length) < 0) {
        perror("Наблюдатель не смог подключиться");
        exit(EXIT_FAILURE);
    }

    update_system_status("Наблюдатель подключен");

    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);
        char buffer[1024];

        ssize_t bytes_received = recvfrom(server_socket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_address, &client_address_length);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Получено подключение от клиента\n");

            // Создание отдельного процесса для обработки клиента
            if (fork() == 0) {
                close(server_socket);
                // Обработка запросов от клиента
                // ...

                exit(0);
            } else {
                close(server_socket);
            }

            // Отправка информации наблюдателю
            char status_message[100];
            snprintf(status_message, sizeof(status_message), "Клиент подключен: %d", client_address.sin_port);
            sendto(observer_socket, status_message, strlen(status_message), 0, (struct sockaddr *)&observer_address, observer_address_length);
        }
    }

    close(observer_socket);
    close(server_socket);

    return 0;
}
```

Код наблюдателя: 
```

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

void display_system_status(const char *status) {
    printf("Наблюдатель: %s\n", status);
}

int main() {
    int observer_socket;
    struct sockaddr_in server_address;

    if ((observer_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Сокет не создан");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &(server_address.sin_addr)) <= 0) {
        perror("Неверный адрес");
        exit(EXIT_FAILURE);
    }

    if (connect(observer_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Соединение не установлено");
        exit(EXIT_FAILURE);
    }

    char status_message[100];
    while (1) {
        memset(status_message, 0, sizeof(status_message));

        // Получение информации от сервера
        if (recv(observer_socket, status_message, sizeof(status_message), 0) <= 0) {
            perror("Получение информации от сервера провалено");
            exit(EXIT_FAILURE);
        }

        display_system_status(status_message);
    }

    close(observer_socket);

    return 0;
}
```



