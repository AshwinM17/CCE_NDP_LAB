#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/tic_tac_toe_socket"
#define MAX_BUF 1024

int main() {
    int client_sock;
    struct sockaddr_un server_addr;
    char message[MAX_BUF];

    // Create Unix domain socket
    if ((client_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // Connect to the server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Receive the prompt and display it
        if (recv(client_sock, message, MAX_BUF, 0) <= 0) {
            perror("Error receiving message");
            break;
        }
        printf("%s", message);

        // Get the move from the user
        int row, col;
        printf("Enter row and column (0-2): ");
        scanf("%d %d", &row, &col);
        sprintf(message, "%d %d", row, col);
        send(client_sock, message, strlen(message) + 1, 0);

        // Receive the result or error
        if (recv(client_sock, message, MAX_BUF, 0) <= 0) {
            perror("Error receiving result");
            break;
        }
        printf("%s\n", message);
    }

    close(client_sock);
    return 0;
}