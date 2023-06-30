#include "buffer/buffer.h"
#include "poller.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <semaphore.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>



int main(int argc, char *argv[]) {
    
    // Check for number of arguments
    if (argc != 6) {
        printf("Invalid number of arguments.\n");
        printf("Please provide portnum numWorkerthreads bufferSize poll-log poll-stats\n");
        return 1;
    }

    // Extract the command-line arguments
    int portnum = atoi(argv[1]);     // Number of port
    int numWorkerthreads = atoi(argv[2]);   // Number of workers 
    if (numWorkerthreads <= 0) {
        printf("Invalid number for numWorkerthreads\n");
        return 1;
    }
    int bufferSize = atoi(argv[3]);     // The size of buffer
    if (bufferSize <= 0) {
        printf("Invalid number for  bufferSize\n");
        return 1;        
    }
    char* pollLog = argv[4];      // The pollLog name
    char* pollStats = argv[5];    // The pollStat name


    // buffer creation
    CircularBuffer buffer;
    buffer_init(&buffer, bufferSize);

    // Set args Data
    ThreadArgs args;
    args.buffer = &buffer;
    args.numWorkerthreads = numWorkerthreads;
    args.portNum = portnum;
    args.bufferSize  = bufferSize;
    args.pollLog = pollLog;
    args.pollStats = pollStats;

    // File
    clean_createEmpty_file(pollLog);
    clean_createEmpty_file(pollStats);

    // Create master thread
    pthread_t master;
    if (pthread_create(&master, NULL, masterThread, (void*)&args) != 0) {
        printf("Error on master thread create.\n");
        return 1;
    }

    // Join master master
    if (pthread_join(master, NULL) != 0) {
        printf("Error on master thread join.\n");
        return 1;
    }

    // Delete buffer 
    free (buffer.buffer);

    return 0;
}



