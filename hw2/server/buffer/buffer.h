#pragma once
#include <pthread.h>
#include <semaphore.h>

// Buffer struct and relative fuctions
typedef struct {
    int* buffer;
    int size;    // buffer size
    int start;   // start pointer
    int end;     // end pointer
    int count;   // num of elements in buffer
    int running; // checks if a buffer has remove but it still exists
    pthread_mutex_t mutex;     // Mutex for safe entry on buffer
    sem_t full;    // Semaphore to track number of full slots in the buffer
    sem_t empty;   // Semaphore to track number of empty slots in the buffer
} CircularBuffer;


void buffer_init(CircularBuffer* buffer, int size);

void buffer_push(CircularBuffer* buffer, int value);

int buffer_pop(CircularBuffer* buffer);

void buffer_runner(CircularBuffer* buffer);
