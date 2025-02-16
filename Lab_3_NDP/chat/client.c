#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define SERVER_IP "172.16.53.136"   // Replace with the server's IP address
#define SERVER_PORT 8080           // Replace with the server's port number
#define BUFFER_SIZE 1024

void handle_error(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    pid_t pid;
    
    // Create a TCP socket (IPv4)
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        handle_error("Socket creation failed");
    }

    // Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    // Convert the IP address from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        handle_error("Invalid IP address");
    }
    
    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        handle_error("Connect failed");
    }

    // Print PID and PPID
    pid = getpid();
    printf("Client PID: %d, PPID: %d\n", pid, getppid());

    // Fork to handle full-duplex communication (chat)
    pid = fork();
    if (pid == 0) {
        // Child process: handles receiving from server
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
            if (bytes_received <= 0) {
                break;
            }
            printf("Server: %s\n", buffer);
        }
        close(client_fd);
        exit(0);
    } else if (pid > 0) {
        // Parent process: handles sending to server
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            printf("Client: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            send(client_fd, buffer, strlen(buffer), 0);
        }
    } else {
        handle_error("Fork failed");
    }

    close(client_fd);
    return 0;
}