#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080  // Use dynamic port assignment
#define MAX_BUFF 1024

// Structure to store student information
struct Student {
    int registration_number;
    char name[50];
    char residential_address[100];
    char department[50];
    int semester;
    char section;
    char courses[200];  // Comma-separated courses
    char subject_code[10];
    int marks;
};

// Function to handle the "Add Student" operation
void add_student(int new_sock, struct Student *database, int *db_size) {
    struct Student student;
    read(new_sock, &student, sizeof(student));  // Receive student details

    // Add student to database
    database[*db_size] = student;
    (*db_size)++;

    send(new_sock, "Student added successfully!", 26, 0);
}

// Function to handle the "Get Student Info" operation
void get_student_info(int new_sock, struct Student *database, int db_size) {
    int reg_no;
    read(new_sock, &reg_no, sizeof(reg_no));  // Receive registration number

    // Search for student in database
    for (int i = 0; i < db_size; i++) {
        if (database[i].registration_number == reg_no) {
            send(new_sock, &database[i], sizeof(struct Student), 0);
            return;
        }
    }

    send(new_sock, "Student not found.", 19, 0);
}

// Function to handle the "Update Student Marks" operation
void update_student_marks(int new_sock, struct Student *database, int db_size) {
    int reg_no, new_marks;
    read(new_sock, &reg_no, sizeof(reg_no));  // Receive registration number
    read(new_sock, &new_marks, sizeof(new_marks));  // Receive new marks

    // Update marks in the database
    for (int i = 0; i < db_size; i++) {
        if (database[i].registration_number == reg_no) {
            database[i].marks = new_marks;
            send(new_sock, "Marks updated successfully!", 26, 0);
            return;
        }
    }

    send(new_sock, "Student not found.", 19, 0);
}

// Main server function
int main() {
    int server_fd, new_sock, db_size = 0;
    struct sockaddr_in address;
    char buffer[MAX_BUFF];
    struct Student database[100];  // Simple database with a max of 100 students

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        if ((new_sock = accept(server_fd, NULL, NULL)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Read client option
        int option;
        read(new_sock, &option, sizeof(option));

        if (option == 1) {
            add_student(new_sock, database, &db_size);
        } else if (option == 2) {
            get_student_info(new_sock, database, db_size);
        } else if (option == 3) {
            update_student_marks(new_sock, database, db_size);
        } else if (option == 4) {
            send(new_sock, "Exiting...", 10, 0);
            close(new_sock);
            break;
        }

        close(new_sock);
    }

    return 0;
}