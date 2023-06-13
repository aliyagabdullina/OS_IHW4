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
