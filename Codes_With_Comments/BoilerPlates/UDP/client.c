#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    socklen_t serv_len = sizeof(serv_addr);

    // Create UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    while (1) {
        printf("Enter message to send (or 'quit' to exit): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline

        if (strcmp(buffer, "quit") == 0) {
            break;
        }

        // Send message to server
        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, serv_len);
        memset(buffer, 0, BUFFER_SIZE);

        printf("Enter second message to send: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline

        // Send second message to server
        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, serv_len);
        memset(buffer, 0, BUFFER_SIZE);

        // Receive response from server
        int bytes_received = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, 
                                      (struct sockaddr *)&serv_addr, &serv_len);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; // Null-terminate the received data
            printf("Received message: %s\n", buffer);
        }
        memset(buffer, 0, BUFFER_SIZE); // Clear the buffer after processing
    }

    close(sock);
    return 0;
}
