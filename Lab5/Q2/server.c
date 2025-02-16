#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define MAX_CLIENTS 5
#define MAX_BUF 1024

// Function to check if two strings are anagrams
int are_anagrams(char *str1, char *str2) {
    int count[256] = {0};
    
    // If lengths are different, they cannot be anagrams
    if (strlen(str1) != strlen(str2)) {
        return 0;
    }
    
    for (int i = 0; i < strlen(str1); i++) {
        count[str1[i]]++;
        count[str2[i]]--;
    }

    for (int i = 0; i < 256; i++) {
        if (count[i] != 0) {
            return 0; // Not anagrams
        }
    }
    return 1; // They are anagrams
}

void handle_client(int client_sock, struct sockaddr_in client_addr) {
    char client_message[MAX_BUF];
    char response[MAX_BUF];
    time_t now;
    struct tm *tm_info;

    // Get the current time and date
    time(&now);
    tm_info = localtime(&now);

    // Display date and time with client address
    printf("Connection from %s:%d at %s", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), asctime(tm_info));

    // Receive two strings from the client
    recv(client_sock, client_message, MAX_BUF, 0);
    printf("Received first string: %s\n", client_message);
    recv(client_sock, client_message, MAX_BUF, 0);
    printf("Received second string: %s\n", client_message);

    // Check if the strings are anagrams
    if (are_anagrams(client_message, client_message)) {
        strcpy(response, "The strings are anagrams.\n");
    } else {
        strcpy(response, "The strings are not anagrams.\n");
    }

    // Send the response to the client
    send(client_sock, response, strlen(response), 0);

    // Close the client socket
    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create server socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the server socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept client connection
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        // Handle client request
        handle_client(client_sock, client_addr);
    }

    // Close the server socket
    close(server_sock);
    return 0;
}