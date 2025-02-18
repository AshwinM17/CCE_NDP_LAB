#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

#define MAXSIZE 50

void main() {
    int sockfd, retval;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr;
    char buff[MAXSIZE];
    int port;
    
    // Take port number as input
    printf("Enter port number: ");
    scanf("%d", &port);
    getchar();  // Clear the newline character left by scanf
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nSocket Creation Error\n");
        return;
    }
    
    printf("Socket created: %i\n", sockfd);
    
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);  // Use the input port number
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        printf("Connection error\n");
        close(sockfd);
        return;
    }
    
    while (1) {
        printf("Enter the text: ");
        fgets(buff, sizeof(buff), stdin);  // Use fgets to allow spaces in input
        buff[strcspn(buff, "\n")] = 0;    // Remove the newline character
        
        sentbytes = send(sockfd, buff, strlen(buff), 0); // Send only the required bytes
        if (sentbytes == -1) {
            printf("Send error\n");
            close(sockfd);
            return;
        }
        
        // Exit the loop if the user types 'exit'
        if (strcmp(buff, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        recedbytes = recv(sockfd, buff, sizeof(buff), 0);
        if (recedbytes == -1) {
            printf("Receive error\n");
            close(sockfd);
            return;
        }

        // Null-terminate the received data
        buff[recedbytes] = '\0';
        printf("Received from server: %s\n", buff);
    }
    
    close(sockfd);
}