#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3002
#define MAX_BUF 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUF];
    int option;
    char filename[MAX_BUF];
    char search_str[MAX_BUF], replace_str[MAX_BUF];

    // Create socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    // Send filename to server
    printf("Enter file name: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0;  // Remove trailing newline
    send(sock_fd, filename, sizeof(filename), 0);

    // Display menu and handle client options
    while (1) {
        int recb = recv(sock_fd, buffer, sizeof(buffer), 0);
        if(recb == -1){
            printf("Receiving error\n");
            close(sock_fd);
            exit(0);
        }
        if (strcmp(buffer, "File not present") == 0) {
            printf("%s\n", buffer);
            close(sock_fd);
            return 0;
        }
        printf("%s\n", buffer);
        printf("Choose option: ");
        scanf("%d", &option);
        getchar();  // Consume newline

        send(sock_fd, &option, sizeof(option), 0);

        switch (option) {
            case 1: // Search
                printf("Enter string to search: ");
                fgets(search_str, sizeof(search_str), stdin);
                search_str[strcspn(search_str, "\n")] = 0;
                send(sock_fd, search_str, sizeof(search_str), 0);
                memset(buffer, 0, sizeof(buffer));
                recv(sock_fd, buffer, sizeof(buffer), 0);
                printf("%s\n", buffer);
                break;

            case 2: // Replace
                printf("Enter string to replace and new string: ");
                fgets(buffer, sizeof(buffer), stdin);
                send(sock_fd, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
                recv(sock_fd, buffer, sizeof(buffer), 0);
                break;

            case 3: // Reorder
                memset(buffer, 0, sizeof(buffer));
                recv(sock_fd, buffer, sizeof(buffer), 0);
                printf("%s\n", buffer);
                break;

            case 4: // Exit
                close(sock_fd);
                return 0;

            default:
                printf("Invalid option\n");
                break;
        }
    }

    return 0;
}