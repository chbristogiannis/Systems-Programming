#include <stdio.h>

#include <string.h>
#include <stdbool.h>

#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>

#define MAX_ARGS 10
#define MAX_SIZE_ARG 100

bool read_line(char*** command, int* numOfCom, bool* is_redirection, bool* is_piped, bool* is_background) {

    printf("in-mysh-now:>");

    // Read Line
    char* read;
    size_t len = 0;
    if (getline(&read, &len, stdin) == -1) {
        perror("getline");
        exit(EXIT_FAILURE);
    }

    *is_redirection = false;
    if (strstr(read, "<") != NULL && strstr(read, ">") != NULL && strstr(read, ">>") != NULL)
        *is_redirection = true;

    *is_piped = false;
    if (strstr(read, "|") != NULL)
        *is_piped = true;

    *is_background = false;
    if (strstr(read, "&") != NULL)
        *is_background = true;

    int i = 0;
    char** words = NULL;
    char *word = strtok(read, " ");
    // printf("Inside%d\n", numOfCom);
    while (word != NULL && strcmp(word, "\n") != 0) {
        // printf("%s\n", word);
        if (i == MAX_ARGS) {
                return false;
        }
        words = realloc(words, (i+1) * sizeof(char *));
        words[i] = strdup(word); 
        i++;
        word = strtok(NULL, " ");
    }
    *numOfCom = i;
    *command = words;
    // printf("Ending\n");

    return true;
}

void execArgs(char** parsed) {
    // Forking a child
    pid_t pid = fork(); 
  
    if (pid == -1) {
        printf("\nFailed forking child..\n");
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command..\n");
        }
        exit(0);
    }
    wait(NULL); 
}


int main (void) {
    
    
    int numOfCom = 0;
    bool is_redirection;
    bool is_piped;
    bool is_background;

    char** command = NULL;
    // char** command = malloc(MAX_ARGS * sizeof(char*));
    // for (int i=0; i < MAX_ARGS; i++)
    //     command[i] = malloc(MAX_SIZE_ARG* sizeof(char*));
    
    int ij = 0;
    while (ij < 3) {
        
        if (!read_line(&command, &numOfCom, &is_redirection, &is_piped, &is_background))
            return EXIT_FAILURE;
        
        if (strcmp(command[0], "reutno\n") == 0)
            break;
        
        printf("%d %d %d %d\n", numOfCom, is_redirection, is_piped, is_background);
        for (int i = 0; i < numOfCom; i++)
            printf("%s\n", command[i]);
        
        if (!is_redirection && !is_piped && !is_background) {
            execArgs(command);
        }


        for (int i = 0; i <numOfCom; i++) {
            free(command[i]);
        }
        free(command);
        ij ++;
    }
    
    printf("Exiting ...\n");
    // for (int i = 0; i < MAX_ARGS; i++) {
    //     free(command[i]);
    // }
    // free(command);
    
    return 0;
}
