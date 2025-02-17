#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/socket_server"

void permute(char *str, int left, int right) {
    if (left == right) {
        printf("Permutation: %s\n", str);
    } else {
        for (int i = left; i <= right; i++) {
            // Swap characters at indices left and i
            char temp = str[left];
            str[left] = str[i];
            str[i] = temp;

            // Recursively generate permutations
            permute(str, left + 1, right);

            // Backtrack by swapping back
            temp = str[left];
            str[left] = str[i];
            str[i] = temp;
        }
    }
}

int main() {
    int server_socket;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[256];

    // Create a UNIX domain datagram socket (UDP-like)
    server_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Prepare the sockaddr_un structure
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // Bind the socket to the server address

    unlink(SOCKET_PATH); 
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        close(server_socket);
        exit(1);
    }

    printf("Server is waiting for messages...\n");

    // Loop to receive messages from client
    while (1) {
        client_len = sizeof(client_addr);
        int n = recvfrom(server_socket, buffer, sizeof(buffer) - 1, 0, 
                         (struct sockaddr *)&client_addr, &client_len);
        if (n < 0) {
            perror("Error receiving message");
            continue;
        }
        buffer[n] = '\0'; // Null-terminate the received string

        printf("Received message from client: %s\n", buffer);

        // Call the permutation function
        permute(buffer, 0, strlen(buffer) - 1);
    }

    // Close the socket (this will never be reached in the infinite loop)
    close(server_socket);
    return 0;
}