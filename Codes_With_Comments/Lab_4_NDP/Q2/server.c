#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

#define SOCKET_PATH "/tmp/dns_socket"  // Path for the Unix domain socket
#define MAX_BUF 1024                   // Maximum buffer size for communication

// Structure to store DNS database entries
typedef struct {
    char domain_name[256];  // Domain name (e.g., example.com)
    char ip_address[16];    // Corresponding IP address
} DNSRecord;

// Function to load the database from file
int load_database(DNSRecord *db) {
    FILE *file = fopen("database.txt", "r");
    if (!file) {
        perror("Unable to open database.txt");
        return -1;
    }

    int i = 0;
    // Read domain names and IP addresses from the file
    while (fscanf(file, "%s %s", db[i].domain_name, db[i].ip_address) != EOF) {
        i++;
    }
    fclose(file);
    return i; // Return the number of entries in the database
}

// Function to handle DNS requests
void handle_dns_request(int client_sock, DNSRecord *db, int db_size) {
    char domain_name[MAX_BUF];
    char ip_address[16];

    // Receive the domain name from the client
    if (recv(client_sock, domain_name, MAX_BUF, 0) <= 0) {
        perror("Failed to receive domain name");
        return;
    }

    // Search for the domain in the database
    int found = 0;
    for (int i = 0; i < db_size; i++) {
        if (strcmp(db[i].domain_name, domain_name) == 0) {
            strcpy(ip_address, db[i].ip_address);
            found = 1;
            break;
        }
    }

    // Send the result back to the client
    if (found) {
        send(client_sock, ip_address, strlen(ip_address) + 1, 0); // Send the IP address
    } else {
        char *error_message = "Domain not found";
        send(client_sock, error_message, strlen(error_message) + 1, 0); // Send error message
    }
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    DNSRecord db[100]; // Assume a max of 100 records
    int db_size;

    // Load DNS database
    db_size = load_database(db);
    if (db_size < 0) {
        exit(EXIT_FAILURE);
    }

    // Create Unix domain socket
    if ((server_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // Bind socket to the address
    unlink(SOCKET_PATH);  // Remove any existing socket file
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("DNS Server is running...\n");

    while (1) {
        client_len = sizeof(client_addr);
        // Accept client connection
        if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) == -1) {
            perror("Accept failed");
            continue;
        }

        printf("Client connected\n");

        // Handle DNS request
        handle_dns_request(client_sock, db, db_size);

        close(client_sock);
    }

    // Clean up
    close(server_sock);
    unlink(SOCKET_PATH);  // Remove the socket file
    return 0;
}
