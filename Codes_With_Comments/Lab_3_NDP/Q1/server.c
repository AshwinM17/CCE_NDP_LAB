#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/chat_socket"  // Define the path for the UNIX domain socket
#define BUFFER_SIZE 256  // Define the size of the buffer for messages

int client_sock;  // Declare client socket globally for use in threads

// Function to handle receiving messages from the client
void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, sizeof(buffer));  // Clear the buffer
        int bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);  // Receive message from client
        if (bytes_received <= 0) {  // Check if client disconnected or error occurred
            printf("Client disconnected or error in receiving data\n");
            break;
        }
        printf("Client: %s", buffer);  // Print the received message
    }
    return NULL;
}

// Function to handle sending messages to the client
void *send_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        fgets(buffer, sizeof(buffer), stdin);  // Get input from user
        send(client_sock, buffer, strlen(buffer), 0);  // Send message to client
    }
    return NULL;
}

int main() {
    int server_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_addr_len;
    pid_t pid, ppid;

    // Creating a UNIX domain socket
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sun_family = AF_UNIX;  // Set the address family to UNIX
    strcpy(server_addr.sun_path, SOCKET_PATH);  // Set the socket path

    // Bind the socket to the address
    unlink(SOCKET_PATH);  // Remove any existing socket file at the path
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 1) == -1) {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server is waiting for a connection...\n");

    // Accept a client connection
    client_addr_len = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_sock == -1) {
        perror("Accept failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Get PID and PPID
    pid = getpid();  // Get process ID
    ppid = getppid();  // Get parent process ID
    printf("Server PID: %d, PPID: %d\n", pid, ppid);

    // Create threads for receiving and sending messages
    pthread_t receive_thread, send_thread;
    pthread_create(&receive_thread, NULL, receive_messages, NULL);
    pthread_create(&send_thread, NULL, send_messages, NULL);

    // Wait for the threads to finish
    pthread_join(receive_thread, NULL);
    pthread_join(send_thread, NULL);

    // Close the sockets
    close(client_sock);
    close(server_sock);

    return 0;
}
