#include <stdio.h>
#include <string.h>
#include "pollSwayer.h"
#include "../common.h"
#include <stdlib.h>
#include <pthread.h>


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
