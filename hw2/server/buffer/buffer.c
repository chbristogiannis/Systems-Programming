#include "buffer.h"

#include <stdlib.h>



void buffer_init(CircularBuffer* buffer, int size) {
    buffer->buffer = (int*)malloc(size * sizeof(int));
    buffer->size = size;
    buffer->start = 0;
    buffer->end = 0;
    buffer->count = 0;
    buffer->running = 0;
    pthread_mutex_init(&buffer->mutex, NULL);
    sem_init(&buffer->full, 0, 0);    // Initialize full semaphore with 0
    sem_init(&buffer->empty, 0, size); // Initialize empty semaphore with buffer size
}

void buffer_push(CircularBuffer* buffer, int value) {
    sem_wait(&buffer->empty);  // Wait until there is at least one empty slot

    pthread_mutex_lock(&buffer->mutex);

    buffer->buffer[buffer->end] = value;
    buffer->end = (buffer->end + 1) % buffer->size;
    buffer->count++;
    buffer->running++;

    pthread_mutex_unlock(&buffer->mutex);

    sem_post(&buffer->full);  // Increase the number of full slots
}

int buffer_pop(CircularBuffer* buffer) {
    
    sem_wait(&buffer->full);   // Wait until there is at least one full slot

    pthread_mutex_lock(&buffer->mutex);

    int value = buffer->buffer[buffer->start];
    buffer->start = (buffer->start + 1) % buffer->size;
    buffer->count--;

    pthread_mutex_unlock(&buffer->mutex);

    sem_post(&buffer->empty);  // Increase the number of empty slots

    return value;
}
void buffer_runner(CircularBuffer* buffer) {
    pthread_mutex_lock(&buffer->mutex);
    buffer->running--;
    pthread_mutex_unlock(&buffer->mutex);
}

