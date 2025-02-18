#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to calculate CRC-16 checksum
unsigned short crc16(const unsigned char *data, size_t length) {
    unsigned short crc = 0xFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;  // 0xA001 is the reversed polynomial for CRC-16
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

int main() {
    int server_fd, new_socket, opt = 1;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char data[] = "Hello, World!"; // Example data

    // Calculate CRC checksum
    unsigned short crc = crc16((unsigned char *)data, strlen(data));
    printf("Calculated CRC: %04X\n", crc);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Allow immediate reuse of the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        // Send data and CRC checksum to receiver
        snprintf(buffer, BUFFER_SIZE, "%s%04X", data, crc);
        send(new_socket, buffer, strlen(buffer), 0);
        printf("Data sent: %s\n", buffer);

        close(new_socket);
    }

    close(server_fd);
    return 0;
}
