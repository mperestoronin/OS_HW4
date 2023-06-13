#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_FLOWERS 40
#define BUF_SIZE 1024

typedef struct {
    int is_being_watered;
    int is_wilted;
} Flower;

Flower flowers[MAX_FLOWERS];

int main(int argc, char *argv[]) {
    int sock, len;
    struct sockaddr_in server, client;
    char buf[BUF_SIZE];

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Unable to create socket");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Unable to bind socket");
        exit(1);
    }

    for (int i = 0; i < MAX_FLOWERS; i++) {
    flowers[i].is_being_watered = 0;
    flowers[i].is_wilted = 0;
}


    while (1) {
        len = sizeof(client);
        if (recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&client, &len) < 0) {
            perror("Unable to receive from client");
            exit(1);
        }

        // Assume the message is a string with the format "<command> <flower index>"
        int flower_index;
        char command[10];
        sscanf(buf, "%s %d", command, &flower_index);

	//printf("Received command %s for flower %d.\n", command, flower_index);


        if (strcmp(command, "wilted") == 0) {
    flowers[flower_index].is_wilted = 1;
    strcpy(buf, "ok");
} else if (strcmp(command, "water") == 0) {
    if (!flowers[flower_index].is_being_watered && flowers[flower_index].is_wilted) {
        flowers[flower_index].is_being_watered = 1;
        strcpy(buf, "proceed");
    } else {
        strcpy(buf, "wait");
    }
} else if (strcmp(command, "done") == 0) {
    flowers[flower_index].is_being_watered = 0;
    flowers[flower_index].is_wilted = 0;
    strcpy(buf, "ok");
}


     // After sending a response to a gardener:
    if (sendto(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&client, len) < 0) {
        perror("Unable to send to client");
        exit(1);
    }

    // Add this line to introduce a small delay:
    struct timespec ts = {.tv_sec = 0, .tv_nsec = 1000000};  // 1 millisecond
    nanosleep(&ts, NULL);
    }

    return 0;
}
