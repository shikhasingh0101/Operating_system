#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024

int main() {
    int server_socket;
    struct sockaddr_in server_address, client_address;
    char buffer[MAX_BUFFER_SIZE];

    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Bind the socket to the server address
    if (bind(server_socket, (const struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP server is listening on port %d\n", PORT);

    while (1) {
        socklen_t len;
        int n;
        printf("Waiting for a message from the client...\n");
        len = sizeof(client_address);

        // Receive message from client
        n = recvfrom(server_socket, (char *)buffer, MAX_BUFFER_SIZE, 0,
                     (struct sockaddr *)&client_address, &len);
        buffer[n] = '\0';

        printf("Received %d bytes from %s:%d\n", n, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        printf("Message from client: %s\n", buffer);

        // Check if client wants to exit
        if (strcmp(buffer, "exit") == 0) {
            printf("Client has exited. Closing server...\n");
            break;
        }
    }

    // Close the socket
    close(server_socket);

    return 0;
}
