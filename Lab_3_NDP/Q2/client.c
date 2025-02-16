#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/socket_server"

int main() {
    int client_socket;
    struct sockaddr_un server_addr;
    char message[256];

    // Create a UNIX domain datagram socket (UDP-like)
    client_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Prepare the sockaddr_un structure
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // Loop to send multiple messages
    while (1) {
        printf("Enter a string to send to the server for permutation (or type 'exit' to quit): ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0'; // Remove newline character if present

        // Exit condition
        if (strcmp(message, "exit") == 0) {
            break;
        }

        // Send the message to the server
        if (sendto(client_socket, message, strlen(message), 0, 
                   (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Message sending failed");
            close(client_socket);
            exit(1);
        }

        printf("Message sent to server: %s\n", message);
    }

    // Close the socket after sending all messages
    close(client_socket);
    return 0;
}

