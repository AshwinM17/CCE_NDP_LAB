#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define MAX_BUF 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char message[MAX_BUF];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Prompt user for two strings
    printf("Enter first string: ");
    fgets(message, MAX_BUF, stdin);
    message[strcspn(message, "\n")] = 0;  // Remove newline character
    send(sock, message, strlen(message) + 1, 0);

    printf("Enter second string: ");
    fgets(message, MAX_BUF, stdin);
    message[strcspn(message, "\n")] = 0;  // Remove newline character
    send(sock, message, strlen(message) + 1, 0);

    // Receive the response from the server
    recv(sock, message, MAX_BUF, 0);
    printf("Server response: %s\n", message);

    // Close the socket
    close(sock);
    return 0;
}