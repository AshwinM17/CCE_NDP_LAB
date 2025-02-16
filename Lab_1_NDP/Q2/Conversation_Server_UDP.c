#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#define MAXSIZE 90

int main() {
    int sockfd, retval, i;
    socklen_t actuallen;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr, clientaddr;

    char buff[MAXSIZE];
    char response[MAXSIZE];
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1) {
        printf("\nSocket creation error\n");
        return 1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        printf("Binding error\n");
        close(sockfd);
        return 1;
    }

    printf("Server is ready and listening...\n");

    for (;;) {
        actuallen = sizeof(clientaddr);
        recedbytes = recvfrom(sockfd, buff, sizeof(buff) - 1, 0, (struct sockaddr*)&clientaddr, &actuallen);

        if (recedbytes == -1) {
            printf("Receiving error\n");
            close(sockfd);
            return 1;
        }

        buff[recedbytes] = '\0'; // Null-terminate the received string
        printf("Received: %s\n", buff);

        // Check for termination command "halt"
        if (strcmp(buff, "halt\n") == 0) {
            printf("Terminating server as 'halt' command received.\n");
            break;
        }
	
        
        // Palindrome computation
        int len = strlen(buff);
        int is_palindrome = 1;
        for (i = 0; i < len / 2; i++) {
            if (buff[i] != buff[len - i - 2]) { // -2 to handle trailing newline
                is_palindrome = 0;
                break;
            }
        }

        // Count vowels
        int vowels[5] = {0}; // Order: a, e, i, o, u
        for (i = 0; i < len; i++) {
            switch (buff[i]) {
                case 'a': case 'A': vowels[0]++; break;
                case 'e': case 'E': vowels[1]++; break;
                case 'i': case 'I': vowels[2]++; break;
                case 'o': case 'O': vowels[3]++; break;
                case 'u': case 'U': vowels[4]++; break;
                default: break;
            }
        }

        // Prepare response
        snprintf(response, sizeof(response),
                 "%s\nLength: %d\nVowels - a:%d e:%d i:%d o:%d u:%d",
                 is_palindrome ? "Palindrome" : "Not Palindrome",
                 len - 1, // Exclude trailing newline
                 vowels[0], vowels[1], vowels[2], vowels[3], vowels[4]);

        // Send result back to client
        sentbytes = sendto(sockfd, response, strlen(response), 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
        if (sentbytes == -1) {
            printf("Sending error\n");
            close(sockfd);
            return 1;
        }

        printf("Result sent: \n%s\n", response);
    }

    close(sockfd);
    return 0;
}

