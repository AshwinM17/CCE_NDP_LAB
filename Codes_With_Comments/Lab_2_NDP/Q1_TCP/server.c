#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3002
#define MAX_BUF 2048

// Function to search for a string in the file
int search_string_in_file(FILE *file, const char *search_str) {
    char line[MAX_BUF];
    int count = 0;
    rewind(file); // Reset the file pointer to the start
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, search_str)) {
            count++;
        }
    }
    return count;
}

// Function to replace a string in the file
FILE *replace_string_in_file(FILE *file, const char *search_str, const char *replace_str) {
    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) return file; // Return the original file if temp file fails

    char line[MAX_BUF];
    int replaced = 0;

    rewind(file); // Reset the file pointer to the start

    while (fgets(line, sizeof(line), file)) {
        char *pos;
        while ((pos = strstr(line, search_str)) != NULL) {
            replaced = 1;
            *pos = '\0'; // Cut the string before the search string
            fprintf(temp_file, "%s%s", line, replace_str);
            strcpy(line, pos + strlen(search_str)); // Move past the replaced string
        }
        fprintf(temp_file, "%s", line);
    }

    fclose(file);
    fclose(temp_file);

    // Remove the original file and rename the temp file
    remove("file.txt");
    rename("temp.txt", "file.txt");

    // Reopen the file in read-write mode and return it
    file = fopen("file.txt", "r+");
    return file;
}

// Function to reorder lines in the file
FILE *reorder_lines_in_file(FILE *file) {
    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) return file; // Return the original file if temp file fails

    char line[MAX_BUF];
    char lines[100][MAX_BUF]; // Assuming the file has <= 100 lines for simplicity
    int line_count = 0;

    rewind(file); // Reset the file pointer to the start

    // Read all lines into the array
    while (fgets(line, sizeof(line), file)) {
        strcpy(lines[line_count++], line);
    }

    // Handle case where the file is empty
    if (line_count == 0) {
        fclose(temp_file);
        return file;
    }

    // Sort the lines using bubble sort
    for (int i = 0; i < line_count - 1; i++) {
        for (int j = i + 1; j < line_count; j++) {
            if (strcmp(lines[i], lines[j]) > 0) {
                char temp[MAX_BUF];
                strcpy(temp, lines[i]);
                strcpy(lines[i], lines[j]);
                strcpy(lines[j], temp);
            }
        }
    }

    // Write sorted lines to the temp file
    for (int i = 0; i < line_count; i++) {
        fprintf(temp_file, "%s", lines[i]);
    }

    // Close files and replace the original file
    fclose(file);
    fclose(temp_file);

    remove("file.txt");
    rename("temp.txt", "file.txt");

    // Reopen the file in read-write mode
    file = fopen("file.txt", "r+");
    return file;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    char buffer[MAX_BUF];
    FILE *file;
    int option;
    char filename[MAX_BUF];
    char search_str[MAX_BUF], replace_str[MAX_BUF];
    int occurrences;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Listen for incoming connections
    listen(server_fd, 3);
    printf("Server listening on port %d...\n", PORT);

    // Accept client connection
    addr_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        perror("Accept failed");
        exit(1);
    }

    // Receive filename from client
    recv(client_fd, filename, sizeof(filename), 0);
    file = fopen(filename, "r+");
    if (!file) {
        send(client_fd, "File not present", 17, 0);
        close(client_fd);
        close(server_fd);
        return 0;
    }

    // Main loop to handle client requests
    while (1) {
        send(client_fd, "1. Search 2. Replace 3. Reorder 4. Exit", 40, 0);
        recv(client_fd, &option, sizeof(option), 0);

        switch (option) {
            case 1: // Search
                send(client_fd, "Enter string to search: ", 23, 0);
                recv(client_fd, search_str, sizeof(search_str), 0);
                search_str[strcspn(search_str, "\n")] = 0;
                occurrences = search_string_in_file(file, search_str);
                if (occurrences > 0) {
                    sprintf(buffer, "String found %d times", occurrences);
                } else {
                    strcpy(buffer, "String not found");
                }
                send(client_fd, buffer, sizeof(buffer), 0);
                break;

            case 2: // Replace
                send(client_fd, "Enter string to replace and new string: ", 39, 0);
                recv(client_fd, buffer, sizeof(buffer), 0);
                sscanf(buffer, "%s %s", search_str, replace_str);
                file = replace_string_in_file(file, search_str, replace_str);
                send(client_fd, "String replaced", 16, 0);
                break;

            case 3: // Reorder
                file = reorder_lines_in_file(file);
                send(client_fd, "File reordered", 15, 0);
                break;

            case 4: // Exit
                fclose(file);
                close(client_fd);
                close(server_fd);
                return 0;

            default:
                send(client_fd, "Invalid option", 15, 0);
                break;
        }
    }

    return 0;
}
