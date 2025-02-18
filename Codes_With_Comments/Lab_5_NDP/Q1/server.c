#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

int client_count = 0;  // Counter for connected clients
int client_sockets[MAX_CLIENTS];  // Array to store client socket descriptors
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for thread-safe operations

// Function to handle client connections
void *client_handler(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char message[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);

    // Get client's IP and port
    getpeername(sock, (struct sockaddr*)&client_addr, &addr_size);
    printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Receive message from the client
    if (recv(sock, message, sizeof(message), 0) > 0) {
        printf("Received: %s\n", message);

        // Append message to the file
        FILE *file = fopen("server_file.txt", "a");
        if (file != NULL) {
            fprintf(file, "%s ", message);
            fclose(file);
        }

        // Send full message back to client
        char full_message[] = "Manipal Institute of Technology";
        send(sock, full_message, sizeof(full_message), 0);

        // Print the message on server side
        printf("Manipal Institute of Technology\n");

        // Send client's IP address back to the client
        send(sock, inet_ntoa(client_addr.sin_addr), strlen(inet_ntoa(client_addr.sin_addr)) + 1, 0);
    }

    close(sock);
    return NULL;
}

int main() {
    int server_sock, client_sock, c;
    struct sockaddr_in server_addr, client_addr;
    pthread_t thread_id;

    // Create server socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set SO_REUSEADDR option
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_sock, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        c = sizeof(struct sockaddr_in);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, (socklen_t*)&c);
        if (client_sock < 0) {
            perror("Client accept failed");
            continue;
        }

        // Thread-safe client count update
        pthread_mutex_lock(&mutex);
        if (client_count >= 2) {
            printf("More than 2 clients connected, terminating session...\n");
            char terminate_msg[] = "terminate session";
            for (int i = 0; i < client_count; i++) {
                send(client_sockets[i], terminate_msg, sizeof(terminate_msg), 0);
                close(client_sockets[i]);
            }
            close(server_sock);
            return 0;
        }
        client_sockets[client_count++] = client_sock;
        pthread_mutex_unlock(&mutex);

        // Create a new thread for client handling
        if (pthread_create(&thread_id, NULL, client_handler, (void*)&client_sock) < 0) {
            perror("Thread creation failed");
            continue;
        }

        pthread_detach(thread_id); // Detach thread to avoid memory leaks
    }

    close(server_sock);
    return 0;
}
