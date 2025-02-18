#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/chat_socket"  // Define the path for the UNIX domain socket
#define BUFFER_SIZE 256  // Define the size of the buffer for messages

int client_sock;  // Declare client socket globally for use in threads

// Function to handle receiving messages from the server
void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, sizeof(buffer));  // Clear the buffer
        int bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);  // Receive message from server
        if (bytes_received <= 0) {  // Check if server disconnected or error occurred
            printf("Server disconnected or error in receiving data\n");
            break;
        }
        printf("Server: %s", buffer);  // Print the received message
    }
    return NULL;
}

// Function to handle sending messages to the server
void *send_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        fgets(buffer, sizeof(buffer), stdin);  // Get input from user
        send(client_sock, buffer, strlen(buffer), 0);  // Send message to server
    }
    return NULL;
}

int main() {
    struct sockaddr_un server_addr;
    pid_t pid, ppid;

    // Creating a UNIX domain socket
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sun_family = AF_UNIX;  // Set the address family to UNIX
    strcpy(server_addr.sun_path, SOCKET_PATH);  // Set the socket path

    // Connect to the server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    // Get PID and PPID
    pid = getpid();  // Get process ID
    ppid = getppid();  // Get parent process ID
    printf("Client PID: %d, PPID: %d\n", pid, ppid);

    // Create threads for receiving and sending messages
    pthread_t receive_thread, send_thread;
    pthread_create(&receive_thread, NULL, receive_messages, NULL);
    pthread_create(&send_thread, NULL, send_messages, NULL);

    // Wait for the threads to finish
    pthread_join(receive_thread, NULL);
    pthread_join(send_thread, NULL);

    // Close the socket
    close(client_sock);

    return 0;
}
