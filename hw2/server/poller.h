#pragma once

#include "buffer/buffer.h"

// The args info using them for the threads
typedef struct {
    CircularBuffer* buffer;
    int numWorkerthreads;
    int portNum;
    int bufferSize;
    char* pollLog;
    char* pollStats;
} ThreadArgs;

// Threads Fuctions
void* masterThread(void* arg);
void* workerThread(void* arg);
