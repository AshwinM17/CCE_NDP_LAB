#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define MAXSIZE 90

void main() {
    int sockfd, retval;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];
    socklen_t clientlen = sizeof(clientaddr);
    int port;
    
    // Take port number as input
    printf("Enter port number: ");
    scanf("%d", &port);
    getchar();  // Clear the newline character left by scanf
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        printf("\nSocket creation error\n");
        return;
    }
    
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);  // Use the input port number
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        printf("Binding error\n");
        close(sockfd);
        return;
    }

    printf("Server is listening on port %d...\n", port);
    
    while (1) {
        // Receive message from the client using recvfrom()
        recedbytes = recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr*)&clientaddr, &clientlen);
        if (recedbytes == -1) {
            printf("Receive error\n");
            break;
        }
        buff[recedbytes] = '\0';  // Null terminate the received string
        printf("Received from client: %s\n", buff);
        
        // If the client sends "exit", break out of the loop
        if (strcmp(buff, "exit") == 0) {
            printf("Client exited\n");
            break;
        }
        
        // Send a response back to the client using sendto()
        printf("Enter a response: ");
        fgets(buff, sizeof(buff), stdin);
        buff[strcspn(buff, "\n")] = 0;  // Remove the newline character

        sentbytes = sendto(sockfd, buff, strlen(buff) + 1, 0, (struct sockaddr*)&clientaddr, clientlen);
        if (sentbytes == -1) {
            printf("Send error\n");
            break;
        }
    }
    
    close(sockfd);
}