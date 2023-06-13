#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char buf[BUF_SIZE];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Unable to create socket");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));

    // Register as spectator
    strcpy(buf, "register_spectator");
    if (sendto(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Unable to send to server");
        exit(1);
    }

    // Wait for messages from server and print them
    while (1) {
        if (recv(sock, buf, BUF_SIZE, 0) < 0) {
            perror("Unable to receive from server");
            exit(1);
        }

        printf("%s\n", buf);
    }

    return 0;
}
