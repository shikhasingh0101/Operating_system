#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024

void reverseString(char *str) {
    int length = strlen(str);
    int i;
    for (i = 0; i < length / 2; i++) {
        char temp = str[i];
        str[i] = str[length - i - 1];
        str[length - i - 1] = temp;
    }
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    char buffer[MAX_BUFFER_SIZE];


    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Bind the socket to the server address
    if (bind(server_socket, (const struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("TCP server is listening on port %d\n", PORT);

    // Accept a client connection
    socklen_t client_address_len = sizeof(client_address);
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len)) == -1) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Client connected: %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    // Receive message from client
    ssize_t bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
    if (bytes_received == -1) {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }
    buffer[bytes_received] = '\0';

    // Reverse the received string
    reverseString(buffer);

    // Send the reversed string back to the client
    if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
        perror("send failed");
        exit(EXIT_FAILURE);
    }

    printf("Reversed string sent to client: %s\n", buffer);

    close(client_socket);
    close(server_socket);

    return 0;
}
