#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define MAX_FLOWERS 40
#define BUF_SIZE 1024

typedef struct {
    int is_wilted;
} Flower;

Flower flowers[MAX_FLOWERS];

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

    // Initialize all flowers to be healthy
    for (int i = 0; i < MAX_FLOWERS; i++) {
        flowers[i].is_wilted = 0;
    }

    // Main loop, every 5 seconds make a flower wilted
    while (1) {
        sleep(5);

        int flower_to_wilt = rand() % MAX_FLOWERS;
        flowers[flower_to_wilt].is_wilted = 1;

        // Send a message to the server indicating that the flower has wilted
        sprintf(buf, "wilted %d", flower_to_wilt);
        if (sendto(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&server, sizeof(server)) < 0) {
            perror("Unable to send to server");
            exit(1);
        }
	printf("Flower %d has wilted.\n", flower_to_wilt);

        // Wait for the response from the server
        if (recv(sock, buf, BUF_SIZE, 0) < 0) {
            perror("Unable to receive from server");
            exit(1);
        }

        // If a gardener has watered the flower, mark it as healthy again
        if (strcmp(buf, "ok") == 0) {
            flowers[flower_to_wilt].is_wilted = 0;
        }
    }

    return 0;
}

