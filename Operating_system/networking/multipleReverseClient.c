#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 12345
#define SERVER_IP "127.0.0.1"
#define MAX_BUFFER_SIZE 1024

void *send_and_receive(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[MAX_BUFFER_SIZE];

    while (1) {
        // Prompt the user to enter a string
        printf("Enter a string ('exit' to quit): ");
        fgets(buffer, MAX_BUFFER_SIZE, stdin);

        // Remove trailing newline character
        buffer[strcspn(buffer, "\n")] = 0;

        // Send the string to the server
        if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
            perror("send failed");
            break;
        }

        // Receive the reversed string from the server
        ssize_t bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
        if (bytes_received == -1) {
            perror("recv failed");
            break;
        }

        // Null-terminate the received string
        buffer[bytes_received] = '\0';

        printf("Reversed string received from server: %s\n", buffer);

        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }
    }

    // Close the client socket
    close(client_socket);
    pthread_exit(NULL);
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    pthread_t thread_id;

    // Create a TCP socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    // Create a thread to send input and receive reversed string
    if (pthread_create(&thread_id, NULL, send_and_receive, &client_socket) != 0) {
        perror("pthread_create failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Wait for the thread to complete
    pthread_join(thread_id, NULL);

    return 0;
}
