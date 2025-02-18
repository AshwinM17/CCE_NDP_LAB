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
    char data[BUF], response[BUF * 2];

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

    // Receive corrected Hamming code and decoded data
    recv(sock, response, sizeof(response), 0);
    
    // Split the response into corrected Hamming code and decoded data
    char *corrected_hamming = strtok(response, "|");
    char *decoded_data = strtok(NULL, "|");

    printf("Corrected Hamming Encoded Data: %s\n", corrected_hamming);
    printf("Decoded (Original) Data: %s\n", decoded_data);

    close(sock);
    return 0;
}
