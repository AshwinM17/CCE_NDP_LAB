#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 8080
#define MAX_BUFFER 1024
#define MAX_STUDENTS 10

// Structure to hold student information
typedef struct
{
    char registration_number[20];
    char name[100];
    char subject_code[10];
} StudentInfo;

// Array to store multiple students' information
StudentInfo students[MAX_STUDENTS];
int student_count = 0;

// Function to add sample student entries
void add_sample_students()
{
    strcpy(students[0].registration_number, "S101");
    strcpy(students[0].name, "Alice");
    strcpy(students[0].subject_code, "CS101");

    strcpy(students[1].registration_number, "S102");
    strcpy(students[1].name, "Bob");
    strcpy(students[1].subject_code, "CS102");

    strcpy(students[2].registration_number, "S103");
    strcpy(students[2].name, "Charlie");
    strcpy(students[2].subject_code, "CS103");

    student_count = 3; // Set the student count
}

// Function to handle client requests
void handle_client(int client_sock)
{
    char buffer[MAX_BUFFER];
    ssize_t bytes_read;
    StudentInfo student_info;

    while (1) // Keep handling requests until the client disconnects
    {
        // Receive the client option
        bytes_read = read(client_sock, buffer, MAX_BUFFER);
        if (bytes_read <= 0)
        {
            printf("Client disconnected\n");
            break;
        }

        int option = atoi(buffer);
        if (option == 0) // Exit option from client
        {
            printf("Client requested to exit.\n");
            break;
        }

        // Receive the student information
        bytes_read = read(client_sock, &student_info, sizeof(StudentInfo));
        if (bytes_read <= 0)
        {
            perror("Error reading student info from client");
            break;
        }

        // Processing based on the selected option
        int found = 0;
        switch (option)
        {
        case 1: // Search by Registration Number
            for (int i = 0; i < student_count; i++)
            {
                if (strcmp(students[i].registration_number, student_info.registration_number) == 0)
                {
                    snprintf(buffer, MAX_BUFFER, "Reg No: %s, Name: %s, Subject Code: %s, PID: %d",
                             students[i].registration_number, students[i].name, students[i].subject_code, getpid());
                    found = 1;
                    break;
                }
            }
            if (!found)
                snprintf(buffer, MAX_BUFFER, "Student not found with Registration Number: %s", student_info.registration_number);
            break;

        case 2: // Search by Name
            for (int i = 0; i < student_count; i++)
            {
                if (strcmp(students[i].name, student_info.name) == 0)
                {
                    snprintf(buffer, MAX_BUFFER, "Name: %s, Reg No: %s, Subject Code: %s, PID: %d",
                             students[i].name, students[i].registration_number, students[i].subject_code, getpid());
                    found = 1;
                    break;
                }
            }
            if (!found)
                snprintf(buffer, MAX_BUFFER, "Student not found with Name: %s", student_info.name);
            break;

        case 3: // Search by Subject Code
            for (int i = 0; i < student_count; i++)
            {
                if (strcmp(students[i].subject_code, student_info.subject_code) == 0)
                {
                    snprintf(buffer, MAX_BUFFER, "Subject Code: %s, Name: %s, Reg No: %s, PID: %d",
                             students[i].subject_code, students[i].name, students[i].registration_number, getpid());
                    found = 1;
                    break;
                }
            }
            if (!found)
                snprintf(buffer, MAX_BUFFER, "Student not found with Subject Code: %s", student_info.subject_code);
            break;

        default:
            snprintf(buffer, MAX_BUFFER, "Invalid Option");
        }

        // Send the response back to the client
        write(client_sock, buffer, strlen(buffer) + 1);
    }

    close(client_sock);
    exit(0); // Terminate child process
}

int main()
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create a TCP socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("Error creating socket");
        exit(1);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error binding socket");
        close(server_sock);
        exit(1);
    }

    // Listen for connections
    if (listen(server_sock, 5) < 0)
    {
        perror("Error listening on socket");
        close(server_sock);
        exit(1);
    }

    // Add sample students
    add_sample_students();

    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0)
        {
            perror("Error accepting client connection");
            continue;
        }

        // Create a child process to handle the client
        pid_t pid = fork();
        if (pid == 0) // Child process
        {
            close(server_sock);
            handle_client(client_sock);
        }
        else if (pid > 0) // Parent process
        {
            close(client_sock);
        }
        else
        {
            perror("Error creating child process");
            close(client_sock);
        }
    }

    close(server_sock);
    return 0;
}
