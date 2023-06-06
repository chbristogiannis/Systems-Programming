// gcc -o pollSwayer pollSwayer.c -lpthread
// ./pollSwayer linux01.di.uoa.gr 5634 inputFile.txt

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_NAME_LENGTH 50
#define MAX_VOTE_LENGTH 30
#define MAX_SERVER_LENGTH 30
#define MAX_VOTERS 100

typedef struct {
    char name[MAX_NAME_LENGTH];
    char vote[MAX_VOTE_LENGTH];
    char serverName[MAX_SERVER_LENGTH];
    int portNum;
} ClientThreadData;

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

int main(int argc, char *argv[]) {

    // Check for number of arguments
    if (argc != 4) {
        printf("Invalid number of arguments.\n");
        printf("Please provide the server name, port number, and input file name.\n");
        return 1;
    }

    // Extract the command-line arguments
    char *serverName = argv[1];
    int portNum = atoi(argv[2]);
    char *fileName = argv[3];
    
    // Open the input file only for reading
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror("Client Error: Failed to open input file");
        return 1;
    }
    
    pthread_t threads[MAX_VOTERS];
    char line[MAX_NAME_LENGTH + MAX_VOTE_LENGTH + 1];
    int numThreads = 0;

    // A vector for all args that every thread needs. We save it in order to free it in the end.
    ClientThreadData** dataVector = malloc(sizeof(ClientThreadData*));

    // Read line from the file
    while (fgets(line, sizeof(line), file) != NULL) {

        // Split the line into three words
        char word1[MAX_NAME_LENGTH];
        char word2[MAX_NAME_LENGTH];
        char word3[MAX_NAME_LENGTH];

        // For every Thread the args info that needs
        ClientThreadData *data = (ClientThreadData*)malloc(sizeof(ClientThreadData));

        // Break the line in three parts/words
        if (sscanf(line, "%s %s %s", word1, word2, word3) == 3) {

            // Combine the first two words with a space
            strncpy(data->name, word1, sizeof(data->name));
            data->name[sizeof(data->name) - 1] = '\0';
            strcat(data->name, " ");
            strncat(data->name, word2, sizeof(data->name) - strlen(data->name) - 1);

            // Copy the third word
            strncpy(data->vote, word3, sizeof(data->vote));
            data->vote[sizeof(data->vote) - 1] = '\0';
            
            // Save rest data
            strncpy(data->serverName, serverName, sizeof(data->vote));
            data->portNum = portNum;

            // Create voteThread 
            if (pthread_create(&threads[numThreads], NULL, voteThread, (void*)data) != 0) {
                perror("Error: Failed to create thread");
                return 1;
            }

        dataVector = (ClientThreadData**)realloc(dataVector, (numThreads + 1) * sizeof(ClientThreadData*));
        dataVector[numThreads] = data;
        numThreads++;
        }
    }

    // Close the inputFile
    fclose(file);
    
    // Join for voteThread
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Free ClientThreadData* 
    for (int i = 0; i < numThreads; i++) {
        free(dataVector[i]);
    }
    free(dataVector); // Free the vector
    
    return 0;
}
