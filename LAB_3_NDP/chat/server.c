#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define SERVER_PORT 8080       // Port to listen on
#define BUFFER_SIZE 1024

void handle_error(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    pid_t pid;
    
    // Create a TCP socket (IPv4)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        handle_error("Socket creation failed");
    }
    
    // Prepare server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Bind to any available network interface
    server_addr.sin_port = htons(SERVER_PORT); // Use port 8080
    
    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        handle_error("Bind failed");
    }

    // Listen for client connections
    if (listen(server_fd, 1) == -1) {
        handle_error("Listen failed");
    }

    printf("Server waiting for connection on port %d...\n", SERVER_PORT);

    // Accept client connection
    client_len = sizeof(client_addr);
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
        handle_error("Accept failed");
    }

    // Print PID and PPID
    pid = getpid();
    printf("Server PID: %d, PPID: %d\n", pid, getppid());

    // Fork to handle full-duplex communication (chat)
    pid = fork();
    if (pid == 0) {
        // Child process: handles receiving from client
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
            if (bytes_received <= 0) {
                break;
            }
            printf("Client: %s\n", buffer);
        }
        close(client_fd);
        exit(0);
    } else if (pid > 0) {
        // Parent process: handles sending to client
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            printf("Server: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            send(client_fd, buffer, strlen(buffer), 0);
        }
    } else {
        handle_error("Fork failed");
    }

    close(server_fd);
    return 0;
}