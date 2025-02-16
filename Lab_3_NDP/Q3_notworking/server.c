#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/wait.h>

#define PORT 8080
#define MAX_BUFF 1024

int compare_char_desc(const void *a, const void *b) {
    return (*(char *)b - *(char *)a);
}

int compare_int_asc(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void process_string(char *str, int new_sock) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork failed");
        close(new_sock);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        char numbers[MAX_BUFF];
        int num_count = 0;

        for (int i = 0; str[i] != '\0'; i++) {
            if (isdigit(str[i])) {
                numbers[num_count++] = str[i];
            }
        }

        numbers[num_count] = '\0';

        qsort(numbers, num_count, sizeof(char), compare_int_asc);

        write(new_sock, numbers, strlen(numbers));
        exit(0);
    } else {
        char chars[MAX_BUFF];
        int char_count = 0;

        for (int i = 0; str[i] != '\0'; i++) {
            if (isalpha(str[i])) {
                chars[char_count++] = str[i];
            }
        }

        chars[char_count] = '\0';

        qsort(chars, char_count, sizeof(char), compare_char_desc);

        write(new_sock, chars, strlen(chars));

        wait(NULL);
    }
}

int main() {
    int server_fd, new_sock;
    struct sockaddr_in address;
    char buffer[MAX_BUFF];

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

    if ((new_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&address)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    read(new_sock, buffer, sizeof(buffer));
    printf("Received from client: %s\n", buffer);

    process_string(buffer, new_sock);

    close(new_sock);
    return 0;
}