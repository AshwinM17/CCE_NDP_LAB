#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define MAXSIZE 90

void main() {
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];
    int port;
    
    // Take port number as input
    printf("Enter port number: ");
    scanf("%d", &port);
    getchar();  // Clear the newline character left by scanf
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
    
    retval = listen(sockfd, 1);
    if (retval == -1) {
        printf("Listen error\n");
        close(sockfd);
        return;
    }
    
    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);
    if (newsockfd == -1) {
        printf("Accept error\n");
        close(sockfd);
        return;
    }
    
    while (1) {
        recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
        if (recedbytes == -1) {
            printf("Receive error\n");
            break;
        }

        // Null-terminate the received data
        buff[recedbytes] = '\0';
        printf("Received from client: %s\n", buff);
        
        // If the client sends "exit", break out of the loop
        if (strcmp(buff, "exit") == 0) {
            printf("Client exited\n");
            break;
        }
        
        printf("Enter a response: ");
        fgets(buff, sizeof(buff), stdin);
        buff[strcspn(buff, "\n")] = 0;  // Remove the newline character

        sentbytes = send(newsockfd, buff, strlen(buff), 0); // Send only the required bytes
        if (sentbytes == -1) {
            printf("Send error\n");
            break;
        }
    }
    
    close(sockfd);
    close(newsockfd);
}