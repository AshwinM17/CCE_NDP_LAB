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
    char message1[MAX_BUF], message2[MAX_BUF], response[MAX_BUF];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Prompt user for two strings
    printf("Enter first string: ");
    fgets(message1, MAX_BUF, stdin);
    message1[strcspn(message1, "\n")] = 0;  // Remove newline character
    send(sock, message1, strlen(message1) + 1, 0);

    printf("Enter second string: ");
    fgets(message2, MAX_BUF, stdin);
    message2[strcspn(message2, "\n")] = 0;  // Remove newline character
    send(sock, message2, strlen(message2) + 1, 0);

    // Receive the response from the server
    recv(sock, response, MAX_BUF, 0);
    printf("Server response: %s\n", response);

    // Close the socket
    close(sock);
    return 0;
}
