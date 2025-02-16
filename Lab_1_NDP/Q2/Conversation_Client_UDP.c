#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#define MAXSIZE 50

int main() {
    int sockfd, retval, i;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];
    char response[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        printf("\nSocket Creation Error\n");
        return 1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    clientaddr.sin_family = AF_INET;
    clientaddr.sin_port = htons(3389);
    clientaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = bind(sockfd, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
    if (retval == -1) {
        printf("Binding error\n");
        close(sockfd);
        return 1;
    }

    for (;;) {
        memset(buff, 0, sizeof(buff));
        printf("Enter the text (or type 'halt' to stop):\n");
        fgets(buff, sizeof(buff), stdin);

        sentbytes = sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
        if (sentbytes == -1) {
            printf("Sending error\n");
            close(sockfd);
            return 1;
        }

        if (strcmp(buff, "halt\n") == 0) {
            printf("Client exiting as 'halt' command sent.\n");
            break;
        }

        memset(response, 0, sizeof(response));
        int size = sizeof(serveraddr);
        recedbytes = recvfrom(sockfd, response, sizeof(response), 0, (struct sockaddr*)&serveraddr, &size);
        if (recedbytes == -1) {
            printf("Receiving error\n");
            close(sockfd);
            return 1;
        }

        printf("Response from server:\n%s\n", response);
    }

    close(sockfd);
    return 0;
}

