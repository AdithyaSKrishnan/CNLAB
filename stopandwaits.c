// stop_and_wait_receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sockfd;
    char buffer[1024];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int expected_seq = 0;
    while (1) {
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                         (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';

        int recv_seq = buffer[0] - '0';
        printf("Received packet: %s | Seq: %d\n", buffer + 1, recv_seq);

        if (recv_seq == expected_seq) {
            printf("Correct packet. Sending ACK %d\n", expected_seq);
            char ack[2];
            ack[0] = expected_seq + '0';
            ack[1] = '\0';
            sendto(sockfd, ack, sizeof(ack), 0, (struct sockaddr *)&cliaddr, len);
            expected_seq = 1 - expected_seq;  // Flip between 0 and 1
        } else {
            printf("Duplicate or out-of-order packet. Resending ACK %d\n", 1 - expected_seq);
            char ack[2];
            ack[0] = (1 - expected_seq) + '0';
            ack[1] = '\0';
            sendto(sockfd, ack, sizeof(ack), 0, (struct sockaddr *)&cliaddr, len);
        }
    }

    close(sockfd);
    return 0;
}
