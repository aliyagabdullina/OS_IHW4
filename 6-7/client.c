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
