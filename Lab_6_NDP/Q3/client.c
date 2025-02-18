#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF 1024

int main() {
    int sock;
    struct sockaddr_in server;
    char data[BUF], response[BUF];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket failed");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Connection failed");
        exit(1);
    }

    // Get user input
    printf("Enter Hamming encoded data: ");
    fgets(data, BUF, stdin);
    data[strcspn(data, "\n")] = 0;  // Remove newline

    // Send data to server
    send(sock, data, strlen(data) + 1, 0);

    // Receive corrected Hamming code
    recv(sock, response, BUF, 0);
    printf("Corrected Hamming Encoded Data: %s\n", response);

    close(sock);
    return 0;
}
