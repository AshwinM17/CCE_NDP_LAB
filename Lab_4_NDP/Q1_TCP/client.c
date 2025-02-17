#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER 1024

// Structure to hold student information
typedef struct
{
    char registration_number[20];
    char name[100];
    char subject_code[10];
} StudentInfo;

int main()
{
    int sock;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER];
    StudentInfo student_info;

    // Create a TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Error creating socket");
        exit(1);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error connecting to server");
        close(sock);
        exit(1);
    }

    while (1)
    {
        printf("\nEnter option (1: Registration Number, 2: Name, 3: Subject Code, 0: Exit): ");
        int option;
        scanf("%d", &option);
        getchar(); // Consume the newline character

        if (option == 0)
        {
            printf("Exiting...\n");
            break;
        }

        snprintf(buffer, sizeof(buffer), "%d", option);
        write(sock, buffer, strlen(buffer) + 1);

        printf("Enter corresponding detail: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (option == 1) strcpy(student_info.registration_number, buffer);
        else if (option == 2) strcpy(student_info.name, buffer);
        else if (option == 3) strcpy(student_info.subject_code, buffer);

        write(sock, &student_info, sizeof(student_info));
        read(sock, buffer, sizeof(buffer));
        printf("Server Response: %s\n", buffer);
    }

    close(sock);
    return 0;
}
