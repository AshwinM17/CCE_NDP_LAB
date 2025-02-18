#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/dns_socket"  // Path for the Unix domain socket
#define MAX_BUF 1024                   // Maximum buffer size for communication

int main() {
    int client_sock;
    struct sockaddr_un server_addr;
    char domain_name[MAX_BUF], ip_address[MAX_BUF];

    // Create Unix domain socket
    if ((client_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // Connect to the server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }

    // Get domain name from user
    printf("Enter domain name to resolve: ");
    fgets(domain_name, MAX_BUF, stdin);
    domain_name[strcspn(domain_name, "\n")] = '\0'; // Remove newline character

    // Send domain name to the server
    send(client_sock, domain_name, strlen(domain_name) + 1, 0);

    // Receive the IP address from the server
    if (recv(client_sock, ip_address, MAX_BUF, 0) > 0) {
        printf("IP Address: %s\n", ip_address);
    } else {
        printf("Failed to resolve domain\n");
    }

    // Close the client socket
    close(client_sock);
    return 0;
}
