// gcc -o poller poller.c -lpthread
// ./poller 5634 4 16 pollLog.txt pollStats.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h> // close

#include <semaphore.h> // for semaphores
#include <pthread.h> // for threads

#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <errno.h> /*EWOULDBLOCK EAGAIN*/

#include <signal.h> /*SIGNT*/
#include <fcntl.h> /*To terminate Master*/

#include <sys/time.h> /**/
#include <sys/types.h> /**/

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
int buffer_runner(CircularBuffer* buffer);



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

//Extra
void updateStats(char* vote, char* filename);
int hasVoted(char* voterName, char* pollLog) ;

// Signal Handlers
volatile sig_atomic_t stop = 0;
void sigint_handler(int signum) {
    stop = 1;  // Set the stop flag to break the while loop
    printf("signit handler\n");
}

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



/////////////////////////////////////////////////////
///        THREADS                                 //
// sigemptyset(&sa.sa_mask);
// sigaddset(&sa.sa_mask, SIGINT);
// Master Thread
void* masterThread(void* arg) {

    Handle Cntl-c signal
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
        char response[512];
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



/////////////////////////////////////////////////////
////        Buffer beahavior                     ////
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

int buffer_runner(CircularBuffer* buffer) {
    pthread_mutex_lock(&buffer->mutex);
    buffer->running--;
    pthread_mutex_unlock(&buffer->mutex);
}


//////////////////////////////////////////////////////////////////////
////   Extra fuctions                                            /////
// Ελένχει αν ο voterName έχει ψηφίσει με το αν βρίσκεται στo αρχείο pollLog
#define MAX_NAME_LENGTH 50
#define MAX_VOTE_LENGTH 30
#define MAX_SERVER_LENGTH 30
#define MAX_LINES 256

int hasVoted(const char* voterName, const char* pollLog) {
    // printf("%s\n", prolLog)
    FILE* file = fopen(pollLog, "r");
    if (file == NULL) {
        perror("has voted error Error opening poll log file");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {

        char word1[MAX_NAME_LENGTH];
        char word2[MAX_NAME_LENGTH];
        char word3[MAX_NAME_LENGTH];

        char name[256];
        sscanf(line, "%s %s %s", word1, word2, word3);
        // Combine the first two words with a space
        strncpy(name, word1, sizeof(name));
        name[sizeof(name) - 1] = '\0';
        strcat(name, " ");
        strncat(name, word2, sizeof(name) - strlen(name) - 1);
        

        // Εάν το όνομα ταιριάζει με το όνομα του ψηφοφόρου, τότε έχει ήδη ψηφίσει
        // printf("%s and %s\n", voterName, name);
        if (strcmp(voterName, name) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

void updateStats(char* vote, char* filename){
    // Open file for reading
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening poll log file\n");
        exit(EXIT_FAILURE);
    }
    char line[100];
    char party[50][MAX_LINES];
    char number_of_votes[50][MAX_LINES];
    int counter = 0;
    int index = -1;
    // Save previous stats
    while (fgets(line, sizeof(line), file) != NULL) {
        // Split the line into two words
        if (sscanf(line, "%s %s", party[counter], number_of_votes[counter]) == 2) {
            if (strcmp(party[counter], vote) == 0) {
                index = counter;
            }
        }
        counter++;
        if (counter >= MAX_LINES) {
            break;
        }
    }
    fclose(file);

    int number_of_votes_int = 0;
    if (index != -1) {
        number_of_votes_int = atoi(number_of_votes[index]);
        number_of_votes_int++;
    }

    // Reopen the file too write the upsated stats
    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening poll log file\n");
        exit(EXIT_FAILURE);
    }

    if(index == -1 && counter < MAX_LINES) {
        fprintf(file, "%s %d\n", vote, 1);
    }

    for(int i = 0; i < counter; i++){
        if (i == index) {
            fprintf(file, "%s %d\n", party[i], number_of_votes_int);
        }
        else {
            fprintf(file, "%s %s\n", party[i], number_of_votes[i]);
        }
    }
    fclose(file);
}