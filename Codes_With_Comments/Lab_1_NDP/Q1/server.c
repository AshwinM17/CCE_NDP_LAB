#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define MAXSIZE 1024 // Define maximum size for arrays and buffers

// Function to search for a number in the array
int search(int *arr, int n, int num) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == num)
            return i;  // Return index of the number if found
    }
    return -1;  // Return -1 if not found
}

// Function to sort the array in ascending order using bubble sort
void sortAscending(int *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (arr[i] > arr[j]) {
                // Swap elements if they are in the wrong order
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

// Function to sort the array in descending order using bubble sort
void sortDescending(int *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (arr[i] < arr[j]) {
                // Swap elements if they are in the wrong order
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

// Function to split the array into odd and even numbers
void splitOddEven(int *arr, int n, int *oddArr, int *evenArr, int *oddCount, int *evenCount) {
    *oddCount = 0;
    *evenCount = 0;

    for (int i = 0; i < n; i++) {
        if (arr[i] % 2 == 0) {
            evenArr[*evenCount] = arr[i];
            (*evenCount)++;
        } else {
            oddArr[*oddCount] = arr[i];
            (*oddCount)++;
        }
    }
}

void main() {
    int sockfd, newsockfd, retval;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t clientlen;
    int arr[MAXSIZE], oddArr[MAXSIZE], evenArr[MAXSIZE];
    int n, choice, num, oddCount, evenCount;
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
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to the server address
    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        perror("Binding error");
        close(sockfd);
        exit(1);
    }

    // Listen for incoming connections
    retval = listen(sockfd, 5);
    if (retval == -1) {
        perror("Listen error");
        close(sockfd);
        exit(1);
    }

    printf("Server listening on port 3388...\n");
    
    // Accept a client connection
    clientlen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &clientlen);
    if (newsockfd == -1) {
        perror("Accept error");
        close(sockfd);
        exit(1);
    }

    while (1) {
        // Receive data from client
        recv(newsockfd, &n, sizeof(int), 0);  // Receive number of integers
        recv(newsockfd, arr, sizeof(int) * n, 0);  // Receive the set of integers
        recv(newsockfd, &choice, sizeof(int), 0);  // Receive operation choice

        // Handle the operations
        switch (choice) {
            case 1:  // Search
                recv(newsockfd, &num, sizeof(int), 0);  // Receive number to search
                int index = search(arr, n, num);
                if (index != -1)
                    sprintf(buffer, "Number %d found at index %d.", num, index);
                else
                    sprintf(buffer, "Number %d not found.", num);
                break;
            case 2:  // Sort in ascending order
                sortAscending(arr, n);
                sprintf(buffer, "Sorted in ascending order: ");
                for (int i = 0; i < n; i++) {
                    char temp[20];
                    sprintf(temp, "%d ", arr[i]);
                    strcat(buffer, temp);
                }
                break;
            case 3:  // Sort in descending order
                sortDescending(arr, n);
                sprintf(buffer, "Sorted in descending order: ");
                for (int i = 0; i < n; i++) {
                    char temp[20];
                    sprintf(temp, "%d ", arr[i]);
                    strcat(buffer, temp);
                }
                break;
            case 4:  // Split into odd and even
                splitOddEven(arr, n, oddArr, evenArr, &oddCount, &evenCount);
                sprintf(buffer, "Odd numbers: ");
                for (int i = 0; i < oddCount; i++) {
                    char temp[20];
                    sprintf(temp, "%d ", oddArr[i]);
                    strcat(buffer, temp);
                }
                strcat(buffer, "\nEven numbers: ");
                for (int i = 0; i < evenCount; i++) {
                    char temp[20];
                    sprintf(temp, "%d ", evenArr[i]);
                    strcat(buffer, temp);
                }
                break;
            default:
                strcpy(buffer, "Invalid choice.");
        }

        // Send the result back to client
        send(newsockfd, buffer, sizeof(buffer), 0);

        // If the user chooses exit, close the connection
        if (choice == 5) {
            printf("Client exited\n");
            break;
        }
    }

    // Close the sockets
    close(newsockfd);
    close(sockfd);
}
