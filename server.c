#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

#define PORT 8080
#define MAXLINE 1024

int main() {
    int sockfd;
    char buffer[MAXLINE];
    char message[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    fd_set readfds;
    int max_sd = sockfd;

    printf("Server ready to chat...\n");

    while (1) {
        FD_ZERO(&readfds);

        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("Select error");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(sockfd, &readfds)) {
            int n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
            buffer[n] = '\0';
            printf("\nClient: %s", buffer);
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            fgets(message, MAXLINE, stdin);
            sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&cliaddr, len);
        }
    }

    close(sockfd);
    return 0;
}
