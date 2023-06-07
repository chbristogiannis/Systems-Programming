#include "poller.h"

#include "helpers/helpers.h"
#include "../common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <errno.h> /*EWOULDBLOCK EAGAIN*/

#include <signal.h> /*SIGNT*/
#include <fcntl.h> /*To terminate Master*/

#include <unistd.h> // close function

// Signal Handlers
volatile sig_atomic_t stop = 0;
void sigint_handler(int signum) {
    stop = 1;  // Set the stop flag to break the while loop
    printf("signit handler\n");
}

/////////////////////////////////////////////////////
///        THREADS                                 //
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
    server.sin_family = AF_INET;    // internet domain
    server.sin_addr . s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (args->portNum);  // given port number
    
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
        // printf("busy waiting\n");
        // Accept connection
        struct sockaddr_in client;
        socklen_t clientLen = sizeof(client);
        // printf("1\n");
        int newsock = accept(sock, (struct sockaddr*)&client, &clientLen);
        // printf("2\n");
        if (newsock < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // Αν ληφθεί το σήμα και διακοπεί το syscall,
                // συνεχίζουμε την επανάληψη του loop
                continue;
            } else {
                perror("accept");
                exit(EXIT_FAILURE);
            }
        }
        buffer_push(args->buffer, newsock);
        printf("Busy waiting\n");
    }
    printf("2.Exiting master server\n");
    
    if (args->buffer->count == 0 && args->buffer->running == 0) {
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
    // printf("1.Exiting master server\n");
    return NULL;
}
// sigemptyset(&sa.sa_mask);
// sigaddset(&sa.sa_mask, SIGINT);


// Worker Thread
pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;
void* workerThread(void* arg) {
    
    // printf("Worker Thread\n");
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
        // printf("I am waiting pos0\n");

        int sock = buffer_pop(buffer);

        // printf("I am waiting pos1\n");

        // Receive voter name
        char voterName[512];
        ssize_t bytesRead = recv(sock, voterName, sizeof(voterName) - 1, 0);
        if (bytesRead <= 0) {
            close(sock);
            continue;
        }
        voterName[bytesRead] = '\0';
        // printf("I am waiting pos2\n");

        // Check if voter has already voted
        pthread_mutex_lock(&fileMutex);
        printf("I am server worker and i receive voter name:%s\n", voterName);
        if (hasVoted(voterName, args->pollLog)) {   // If he has voted exit
            pthread_mutex_unlock(&fileMutex);
            char* request = "ALREADY VOTED";
            send(sock, request, strlen(request), 0);
            close(sock);
            continue;
        }

        // Sent info to receive 
        char* request = "SEND VOTE PLEASE";
        if (send(sock, request, strlen(request), 0) == -1) {
            perror("Server Error: Failed to send request");
            exit(EXIT_FAILURE);
        }

        // Receive vote
        char vote[512];
        bytesRead = recv(sock, vote, sizeof(vote) - 1, 0);
        if (bytesRead <= 0) {
            close(sock);
            continue;
        }
        vote[bytesRead] = '\0';
        // printf("I am server worker and i receive vote: %s\n", vote);
        
        // Record voter and vote to the file poll log
        FILE* file = fopen(args->pollLog, "a");
        if (file == NULL) {
            perror("Error opening poll log file\n");
            close(sock);
            exit(EXIT_FAILURE);
        }
        // printf("I am a server worker let s write to the file\n");
        // pthread_mutex_lock(&fileMutex);
        updateStats(vote, args->pollStats);
        fprintf(file, "%s %s\n", voterName, vote);
        pthread_mutex_unlock(&fileMutex);
        fclose(file);

        // Send final message
        char response[MAX_VOTE_LENGTH];
        snprintf(response, sizeof(response), "VOTE for Party %s RECORDED", vote);
        if (send(sock, response, strlen(response), 0) == -1) {
            perror("Server Error: Failed to send request");
            exit(EXIT_FAILURE);
        }

        close(sock);
        buffer_runner(args->buffer);
    }
    return NULL;
}






