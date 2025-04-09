// stop_and_wait_sender.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>

#define PORT 8080
#define TIMEOUT_SEC 2

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    socklen_t len = sizeof(servaddr);

    char *messages[] = {"Hello", "World", "Stop-and-Wait", "Protocol", "Done"};
    int num_msgs = sizeof(messages) / sizeof(messages[0]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set timeout for receiving ACKs
    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int seq = 0;
    for (int i = 0; i < num_msgs; ) {
        char packet[1024];
        packet[0] = seq + '0';  // Add sequence number at the beginning
        strcpy(packet + 1, messages[i]);

        printf("Sending packet: %s | Seq: %d\n", messages[i], seq);
        sendto(sockfd, packet, strlen(packet), 0,
               (const struct sockaddr *)&servaddr, sizeof(servaddr));

        char ack[2];
        int n = recvfrom(sockfd, ack, sizeof(ack), 0,
                         (struct sockaddr *)&servaddr, &len);

        if (n > 0 && ack[0] - '0' == seq) {
            printf("Received ACK %d\n\n", seq);
            seq = 1 - seq;
            i++;
        } else {
            printf("Timeout or wrong ACK. Resending packet...\n\n");
        }
    }

    close(sockfd);
    return 0;
}
