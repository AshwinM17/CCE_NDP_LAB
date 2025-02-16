#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFF 1024

int main() {
    int sock = 0;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFF];
    char input_string[MAX_BUFF];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    printf("Enter the alphanumeric string: ");
    fgets(input_string, sizeof(input_string), stdin);
    input_string[strcspn(input_string, "\n")] = 0;

    send(sock, input_string, strlen(input_string), 0);

    int num_bytes = read(sock, buffer, sizeof(buffer) - 1);
    if (num_bytes > 0) {
        buffer[num_bytes] = '\0';
        printf("Sorted numbers from server: %s\n", buffer);
    } else {
        perror("Failed to read sorted numbers");
    }

    num_bytes = read(sock, buffer, sizeof(buffer) - 1);
    if (num_bytes > 0) {
        buffer[num_bytes] = '\0';
        printf("Sorted characters from server: %s\n", buffer);
    } else {
        perror("Failed to read sorted characters");
    }

    close(sock);
    return 0;
}