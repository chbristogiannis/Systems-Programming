#include "helpers.h"
#include "../../common.h"

#include "stdio.h"
#include "stdlib.h"
#include <unistd.h> // close
#include <string.h>

int hasVoted(char* voterName, char* pollLog) {
    
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


void clean_createEmpty_file(char* prev) {
    
    // Open the file for reading and writing
    FILE *file = fopen(prev, "r+");

    // Check if the file exists
    if (file == NULL) {     // File doesn't exist, create it
        file = fopen(prev, "w");
        if (file == NULL) {
            printf("Error creating the file.\n");
            exit(EXIT_FAILURE);
        }
        printf("File created: %s\n", prev);
    } else {        // File exists, check if it's empty
        fseek(file, 0, SEEK_END);
        if (ftell(file) == 0) {     // File is empty
            printf("File is already empty.\n");
        } else {        // File is not empty, truncate it
            freopen(prev, "w", file);
            printf("File emptied: %s\n", prev);
        }
    }

    // Close the file
    fclose(file);

}
