#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFF 1024

// Structure to store student information
struct Student {
    int registration_number;
    char name[50];
    char residential_address[100];
    char department[50];
    int semester;
    char section;
    char courses[200];
    char subject_code[10];
    int marks;
};

void add_student(int sock) {
    struct Student student;
    printf("Enter Registration Number: ");
    scanf("%d", &student.registration_number);
    getchar();  // Consume newline character
    printf("Enter Name: ");
    fgets(student.name, sizeof(student.name), stdin);
    student.name[strcspn(student.name, "\n")] = '\0';  // Remove newline
    printf("Enter Residential Address: ");
    fgets(student.residential_address, sizeof(student.residential_address), stdin);
    student.residential_address[strcspn(student.residential_address, "\n")] = '\0';
    printf("Enter Department: ");
    fgets(student.department, sizeof(student.department), stdin);
    student.department[strcspn(student.department, "\n")] = '\0';
    printf("Enter Semester: ");
    scanf("%d", &student.semester);
    getchar();  // Consume newline character
    printf("Enter Section: ");
    scanf("%c", &student.section);
    getchar();
    printf("Enter Courses (comma separated): ");
    fgets(student.courses, sizeof(student.courses), stdin);
    student.courses[strcspn(student.courses, "\n")] = '\0';
    printf("Enter Subject Code: ");
    scanf("%s", student.subject_code);
    printf("Enter Marks: ");
    scanf("%d", &student.marks);

    send(sock, &student, sizeof(student), 0);  // Send student details to server
}

void get_student_info(int sock) {
    int reg_no;
    printf("Enter Registration Number: ");
    scanf("%d", &reg_no);
    send(sock, &reg_no, sizeof(reg_no), 0);  // Send registration number to server

    char buffer[MAX_BUFF];
    int num_bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (num_bytes > 0) {
        buffer[num_bytes] = '\0';
        printf("Student Info: %s\n", buffer);
    }
}

void update_student_marks(int sock) {
    int reg_no, new_marks;
    printf("Enter Registration Number: ");
    scanf("%d", &reg_no);
    printf("Enter New Marks: ");
    scanf("%d", &new_marks);

    send(sock, &reg_no, sizeof(reg_no), 0);  // Send registration number to server
    send(sock, &new_marks, sizeof(new_marks), 0);  // Send new marks to server

    char buffer[MAX_BUFF];
    int num_bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (num_bytes > 0) {
        buffer[num_bytes] = '\0';
        printf("Response: %s\n", buffer);
    }
}

int main() {
    int sock;
    struct sockaddr_in server_addr;

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

    int option;
    while (1) {
        printf("Select an option:\n");
        printf("1. Add Student\n");
        printf("2. Get Student Info\n");
        printf("3. Update Marks\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &option);

        send(sock, &option, sizeof(option), 0);  // Send option to server

        if (option == 1) {
            add_student(sock);
        } else if (option == 2) {
            get_student_info(sock);
        } else if (option == 3) {
            update_student_marks(sock);
        } else if (option == 4) {
            close(sock);
            break;
        }
    }

    return 0;
}