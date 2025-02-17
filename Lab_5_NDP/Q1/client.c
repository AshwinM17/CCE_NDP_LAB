#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];
    char server_response[BUFFER_SIZE];
    char client_message[] = "Institute Of"; // Change this for the second client to "Technology"

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Send message to server
    send(sock, client_message, strlen(client_message) + 1, 0);

    // Receive response from server
    if (recv(sock, server_response, sizeof(server_response), 0) > 0) {
        printf("Server response: %s\n", server_response);
    }

    // Close the socket
    close(sock);
    return 0;
}