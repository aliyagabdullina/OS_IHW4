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
