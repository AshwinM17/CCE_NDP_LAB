#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t client_len = sizeof(client_addr);

    // Create UDP socket
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening on port %d\n", PORT);

    while (1) {
        int bytes_received;

        // Receive data from client
        bytes_received = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, 
                                  (struct sockaddr *)&client_addr, &client_len);
        if (bytes_received < 0) {
            perror("recvfrom failed");
            continue;
        }
        buffer[bytes_received] = '\0';
        printf("Received 1: %s\n", buffer);

        // Receive second message
        bytes_received = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, 
                                  (struct sockaddr *)&client_addr, &client_len);
        if (bytes_received < 0) {
            perror("recvfrom failed");
            continue;
        }
        buffer[bytes_received] = '\0';
        printf("Received 2: %s\n", buffer);

        // Get message to send
        printf("Message to send: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // Remove newline

        // Send response back to client
        ssize_t bytes_sent = sendto(server_fd, buffer, strlen(buffer), 0, 
                                    (struct sockaddr *)&client_addr, client_len);
        if (bytes_sent < 0) {
            perror("sendto failed");
        } else {
            printf("Sent message: %s\n", buffer);
        }
    }

    close(server_fd);
    return 0;
}
