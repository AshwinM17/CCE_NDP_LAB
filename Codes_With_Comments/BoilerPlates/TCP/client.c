#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define ROWS 3
#define COLS 3

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    int matrix[ROWS][COLS] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Send matrix dimensions
    int dims[2] = {ROWS, COLS};
    send(sock, dims, sizeof(dims), 0);

    // Send matrix
    for (int i = 0; i < ROWS; i++) {
        send(sock, matrix[i], COLS * sizeof(int), 0);
    }

    printf("Matrix sent to server.\n");

    close(sock);
    return 0;
}
