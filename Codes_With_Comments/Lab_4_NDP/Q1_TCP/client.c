#include <stdio.h>      // For standard input/output operations
#include <stdlib.h>     // For general purpose functions like exit()
#include <string.h>     // For string manipulation functions
#include <unistd.h>     // For POSIX operating system API
#include <arpa/inet.h>  // For internet operations

#define PORT 8080       // Define the port number to connect to
#define MAX_BUFFER 1024 // Define the maximum buffer size

// Structure to hold student information
typedef struct
{
    char registration_number[20]; // To store registration number
    char name[100];               // To store student name
    char subject_code[10];        // To store subject code
} StudentInfo;

int main()
{
    int sock;                     // Socket file descriptor
    struct sockaddr_in server_addr; // Server address structure
    char buffer[MAX_BUFFER];      // Buffer for sending/receiving data
    StudentInfo student_info;     // Instance of StudentInfo structure

    // Create a TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Error creating socket");
        exit(1);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr)); // Clear server address structure
    server_addr.sin_family = AF_INET;             // Set address family to IPv4
    server_addr.sin_port = htons(PORT);           // Set port number (in network byte order)
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Set IP address to localhost

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error connecting to server");
        close(sock);
        exit(1);
    }

    while (1)
    {
        // Prompt user for option
        printf("\nEnter option (1: Registration Number, 2: Name, 3: Subject Code, 0: Exit): ");
        int option;
        scanf("%d", &option);
        getchar(); // Consume the newline character

        if (option == 0)
        {
            printf("Exiting...\n");
            break;
        }

        // Convert option to string and send to server
        snprintf(buffer, sizeof(buffer), "%d", option);
        write(sock, buffer, strlen(buffer) + 1);

        // Prompt user for search detail
        printf("Enter corresponding detail: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character

        // Store the input in the appropriate field of student_info
        if (option == 1) strcpy(student_info.registration_number, buffer);
        else if (option == 2) strcpy(student_info.name, buffer);
        else if (option == 3) strcpy(student_info.subject_code, buffer);

        // Send student_info to server
        write(sock, &student_info, sizeof(student_info));

        // Receive and display server response
        read(sock, buffer, sizeof(buffer));
        printf("Server Response: %s\n", buffer);
    }

    close(sock); // Close the socket
    return 0;
}
