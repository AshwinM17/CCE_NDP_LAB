#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

#define MAXSIZE 50

void main() {
    int sockfd, retval;
    int sentbytes, recedbytes;
    struct sockaddr_in serveraddr;
    char buff[MAXSIZE];
    int port;
    
    // Take port number as input
    printf("Enter port number: ");
    scanf("%d", &port);
    getchar();  // Clear the newline character left by scanf
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        printf("\nSocket Creation Error\n");
        return;
    }
    
    printf("Socket created: %i\n", sockfd);
    
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);  // Use the input port number
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1) {
        printf("Enter the text: ");
        fgets(buff, sizeof(buff), stdin);  // Use fgets to allow spaces in input
        buff[strcspn(buff, "\n")] = 0;    // Remove the newline character
        
        // Send the message to the server using sendto()
        sentbytes = sendto(sockfd, buff, strlen(buff) + 1, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
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

        // Receive a response from the server using recvfrom()
        recedbytes = recvfrom(sockfd, buff, sizeof(buff), 0, NULL, NULL);
        if (recedbytes == -1) {
            printf("Receive error\n");
            close(sockfd);
            return;
        }
        
        printf("Received from server: %s\n", buff);
    }
    
    close(sockfd);
}