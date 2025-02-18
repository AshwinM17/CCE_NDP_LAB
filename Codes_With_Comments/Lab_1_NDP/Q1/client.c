#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define MAXSIZE 1024 // Define maximum size for arrays and buffers

void main() {
    int sockfd, retval;
    struct sockaddr_in serveraddr;
    int arr[MAXSIZE], n, choice, num;
    char buffer[MAXSIZE];

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation error");
        exit(1);
    }

    // Set up server address structure
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        perror("Connection error");
        close(sockfd);
        exit(1);
    }

    while (1) {
        // Get input from user
        printf("\nEnter the number of integers: ");
        scanf("%d", &n);

        printf("Enter %d integers:\n", n);
        for (int i = 0; i < n; i++) {
            scanf("%d", &arr[i]);
        }

        // Display menu and get user choice
        printf("\nChoose an operation:\n");
        printf("1. Search for a number\n");
        printf("2. Sort in ascending order\n");
        printf("3. Sort in descending order\n");
        printf("4. Split into odd and even\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        // Send data to server
        send(sockfd, &n, sizeof(int), 0);  // Send the number of integers
        send(sockfd, arr, sizeof(int) * n, 0);  // Send the integers array
        send(sockfd, &choice, sizeof(int), 0);  // Send the operation choice

        if (choice == 1) {
            printf("Enter number to search: ");
            scanf("%d", &num);
            send(sockfd, &num, sizeof(int), 0);  // Send the number to search
        }

        // Receive the result from the server
        recv(sockfd, buffer, sizeof(buffer), 0);
        printf("Server response: %s\n", buffer);

        // Exit if user chose to
        if (choice == 5) {
            printf("Exiting...\n");
            break;
        }
    }

    // Close the socket
    close(sockfd);
}
