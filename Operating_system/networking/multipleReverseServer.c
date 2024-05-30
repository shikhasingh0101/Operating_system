#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

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

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[MAX_BUFFER_SIZE];

    while (1) {
        // Receive message from client
        ssize_t bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
        if (bytes_received == -1) {
            perror("recv failed");
            close(client_socket);
            pthread_exit(NULL);
        }

        // Null-terminate the received string
        buffer[bytes_received] = '\0';

        printf("Received %zd bytes from client\n", bytes_received);

        // Check if client wants to exit
        if (strcmp(buffer, "exit") == 0) {
            printf("Client has exited. Closing client socket...\n");
            break;
        }

        // Reverse the received string
        reverseString(buffer);

        // Send the reversed string back to the client
        if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
            perror("send failed");
            break;
        }

        printf("Reversed string sent to client: %s\n", buffer);
    }

    // Close the client socket
    close(client_socket);
    pthread_exit(NULL);
}

int main() {
    int server_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    // Create a TCP socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
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

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("TCP server is listening on port %d\n", PORT);

    while (1) {
        // Accept a client connection
        int client_socket;
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len)) == -1) {
            perror("accept failed");
            continue;
        }

        printf("Client connected: %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        // Create a new thread to handle the client
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, &client_socket) != 0) {
            perror("pthread_create failed");
            close(client_socket);
            continue;
        }

        // Detach the thread to avoid memory leaks
        pthread_detach(thread_id);
    }

    // Close the server socket (this part of the code is unreachable)
    close(server_socket);

    return 0;
}
