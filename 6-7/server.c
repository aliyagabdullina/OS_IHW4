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
