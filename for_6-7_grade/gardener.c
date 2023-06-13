#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_FLOWERS 40
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char buf[BUF_SIZE];
    int gardener_id;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <gardener_id> <ip> <port>\n", argv[0]);
        exit(1);
    }

gardener_id = atoi(argv[1]);

sock = socket(AF_INET, SOCK_DGRAM, 0);
if (sock < 0) {
    perror("Unable to create socket");
    exit(1);
}

server.sin_family = AF_INET;
server.sin_addr.s_addr = inet_addr(argv[2]);   // Use argv[2] for IP address
server.sin_port = htons(atoi(argv[3]));       // Use argv[3] for port

    // Main loop, every 3 seconds try to water a random flower
while (1) {
    sleep(10);

    while (1) { 
        int flower_to_water = rand() % MAX_FLOWERS;

        // Send a request to the server to water the flower
         sprintf(buf, "water %d %d", flower_to_water, gardener_id);
        if (sendto(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&server, sizeof(server)) < 0) {
            perror("Unable to send to server");
            exit(1);
        }


        // Wait for the response from the server
        if (recv(sock, buf, BUF_SIZE, 0) < 0) {
            perror("Unable to receive from server");
            exit(1);
        }

        // If the server gave permission to water the flower, do so
        if (strcmp(buf, "proceed") == 0) {
            printf("Watering flower %d.\n", flower_to_water);

            // Simulate watering the flower by sleeping for a second
            sleep(1);

            // Then send a message to the server indicating that the flower has been watered
            sprintf(buf, "done %d %d", flower_to_water, gardener_id);
            if (sendto(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&server, sizeof(server)) < 0) {
                perror("Unable to send to server");
                exit(1);
            }

            break; 
        }
    }
}

    return 0;
}

