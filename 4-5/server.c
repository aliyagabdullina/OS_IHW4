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
