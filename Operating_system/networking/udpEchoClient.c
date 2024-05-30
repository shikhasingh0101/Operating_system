#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 12345
#define SERVER_IP "127.0.0.1"
#define MAX_BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[MAX_BUFFER_SIZE];

    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    while (1) {
        printf("Enter a message ('exit' to quit): ");
        fgets(buffer, MAX_BUFFER_SIZE, stdin);

        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        if (sendto(client_socket, buffer, strlen(buffer), 0,
                   (const struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
            perror("sendto failed");
            continue;
        }

        struct sockaddr_in server_echo_address;
        socklen_t server_echo_address_len = sizeof(server_echo_address);
        ssize_t bytes_received = recvfrom(client_socket, buffer, MAX_BUFFER_SIZE, 0,
                                           (struct sockaddr *)&server_echo_address, &server_echo_address_len);
        if (bytes_received == -1) {
            perror("recvfrom failed");
            continue;
        }

        buffer[bytes_received] = '\0';

        printf("Echo from server (%s:%d): %s\n", inet_ntoa(server_echo_address.sin_addr),ntohs(server_echo_address.sin_port), buffer);
    }

    close(client_socket);

    return 0;
}
