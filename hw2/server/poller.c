#include "poller.h"
#include "helpers/helpers.h"
#include "../common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

// Signal Handlers
volatile sig_atomic_t stop = 0;
void sigint_handler(int signum) {
    stop = 1;  // Set the stop flag to break the while loop
}


// Master Thread
void* masterThread(void* arg) {

    // Handle Cntl-c signal
    signal(SIGINT, sigint_handler);

    // Args
    ThreadArgs* args = (ThreadArgs*)arg;

    // Create worker threads
    pthread_t workerThreads[args->numWorkerthreads];
    for (int i = 0; i < args->numWorkerthreads; i++) {
        if (pthread_create(&workerThreads[i], NULL, workerThread, arg) != 0) {
            perror("Error creating worker thread");
            return NULL;
        }
    }

    // Create a server socket
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0) ) < 0) {
        printf("Error socket.\n");
        return NULL;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr . s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (args->portNum);
    
    //  Bind socket to address
    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Error bind.\n");
        return NULL;
    }

    // listen for connections
    if (listen(sock, 5) < 0) {
        printf("Error listen.\n");
        return NULL;
    }

    // Not blockable socket
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    // While not Cntr-c pressed the server accept conection and pushes it to the buffer
    while (!stop) {
        // Accept connection
        struct sockaddr_in client;
        socklen_t clientLen = sizeof(client);

        int newsock = accept(sock, (struct sockaddr*)&client, &clientLen);
        if (newsock < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                continue;
            } else {
                perror("accept");
                exit(EXIT_FAILURE);
            }
        }
        buffer_push(args->buffer, newsock);
    }

    if (args->buffer->running == 0) {
        for (int i = 0; i < args->numWorkerthreads; i++) {
            if (pthread_cancel(workerThreads[i]) != 0) {
                printf("Error on worker thread cancel.\n");
                return NULL;
            }
        }
    }
    // Join worker threads
    for (int i = 0; i < args->numWorkerthreads; i++) {
        if (pthread_join(workerThreads[i], NULL) != 0) {
            printf("Error on worker thread join.\n");
            return NULL;
        }
    }

    return NULL;
}


// Worker Thread
pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;
void* workerThread(void* arg) {

    ThreadArgs* args = (ThreadArgs*)arg;
    CircularBuffer* buffer = args->buffer;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    if (pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0) {
        perror("Error setting signal mask");
        return NULL;
    }

    while (!stop) {

        int sock = buffer_pop(buffer);

        // Receive voter name
        char voterName[512];
        ssize_t bytesRead = recv(sock, voterName, sizeof(voterName) - 1, 0);
        if (bytesRead <= 0) {
            close(sock);
            buffer_runner(args->buffer);
            continue;
        }
        voterName[bytesRead] = '\0';

        // Check if voter has already voted
        pthread_mutex_lock(&fileMutex);
        if (hasVoted(voterName, args->pollLog)) {   // If he has voted exit
            pthread_mutex_unlock(&fileMutex);
            char* request = "ALREADY VOTED";
            send(sock, request, strlen(request), 0);
            close(sock);
            buffer_runner(args->buffer);
            continue;
        }

        // Sent info to receive 
        char* request = "SEND VOTE PLEASE";
        if (send(sock, request, strlen(request), 0) == -1) {
            perror("Server Error: Failed to send request");
            close(sock);
            exit(EXIT_FAILURE);
        }

        // Receive vote
        char vote[512];
        bytesRead = recv(sock, vote, sizeof(vote) - 1, 0);
        if (bytesRead <= 0) {
            close(sock);
            buffer_runner(args->buffer);
            continue;
        }
        vote[bytesRead] = '\0';
        
        // Record voter and vote to the file poll log
        FILE* file = fopen(args->pollLog, "a");
        if (file == NULL) {
            perror("Error opening poll log file\n");
            close(sock);
            exit(EXIT_FAILURE);
        }

        updateStats(vote, args->pollStats);
        fprintf(file, "%s %s\n", voterName, vote);
        pthread_mutex_unlock(&fileMutex);
        fclose(file);

        // Send final message
        char response[MAX_VOTE_LENGTH];
        snprintf(response, sizeof(response), "VOTE for Party %s RECORDED", vote);
        if (send(sock, response, strlen(response), 0) == -1) {
            perror("Server Error: Failed to send request");
            close(sock);
            exit(EXIT_FAILURE);
        }

        close(sock);
        buffer_runner(args->buffer);
    }

    return NULL;
}






