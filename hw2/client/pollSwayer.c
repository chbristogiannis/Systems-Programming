// gcc -o pollSwayer pollSwayer.c -lpthread
// ./pollSwayer linux01.di.uoa.gr 5634 inputFile.txt
#include "pollSwayer.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>



void *voteThread(void *arg) {

    printf("Client thread is created with id %ld\n", pthread_self());

    ClientThreadData *data = (ClientThreadData*)arg;
    // printf("Data is %s ", data->name);
    // printf("Data is %s ", data->serverName);
    // printf("Data is %s\n", data->vote);
    
    // In order to connect
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Client Error: Failed to create socket");
        exit(1);
    }
    
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct hostent *rem;
    
    server.sin_family = AF_INET;
    if ((rem = gethostbyname(data->serverName)) == NULL) {
        printf("Client Error: Invalid server address: %s\n", data->serverName);
        exit(1);
    }
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(data->portNum);
    
    if (connect(sock, serverptr, sizeof(server)) < 0) {
        perror("Error: Failed to connect to the server");
        exit(1);
    }

    // Sent Voter Name
    printf("I am client and i have connected, i sent you the voter: %s\n", data->name);
    if (send(sock, data->name, strlen(data->name), 0) == -1) {
        perror("Client Error: Failed to send voter");
        exit(1);
    }

    // Receive Info
    char read[256];
    ssize_t bytesRead = recv(sock, read, sizeof(read) - 1, 0);
    if (bytesRead <= 0) {
        close(sock);
    }
    // printf("Client you sent me: %s\n", read);
    if (strcmp(read, "ALREADY VOTED") == 0) {
        printf("EXITING BECAUSE ALREADY VOTED\n");
        close(sock);
        pthread_exit(NULL);
    }

    // Sent the Vote
    // printf("I am client, i sent you the vote: %s\n", data->vote);
    if (send(sock, data->vote, strlen(data->vote), 0) == -1) {
        perror("Client Error: Failed to send vote");
        exit(EXIT_FAILURE);
    }

    // Receive the goodbye
    // printf("I am client, and i receive your goodbye\n");
    char goodbye[512];
    bytesRead = recv(sock, goodbye, sizeof(goodbye) - 1, 0);
    if (bytesRead <= 0) {
        close(sock);
        printf("Client bytes read 2\n");
        exit(EXIT_FAILURE);
    }

    // printf("Client It was sent succesfully ending client thread\n");
    
    close(sock);

    return NULL;
}

