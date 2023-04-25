#include <stdio.h>

#include <string.h>
#include <stdbool.h>

#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>

#define MAX_ARGS 10
#define MAX_SIZE_ARG 100

bool readLine(char*** command, int* numOfCom, bool* is_redirection, bool* is_piped, bool* is_background) ;
void execSimple(char** parsed);

void execRedirection(char** command, int numOfCom) {
    pid_t pid = fork(); 
  
    if (pid == -1) {
        printf("\nFailed forking child..\n");
        return;
    } else if (pid == 0) {
        char* input_file = NULL;
        char *output_file = NULL;
        char* append_file = NULL;
        for (int i=1; i < numOfCom; i++) {
            printf("%s\n", command[i]);
            if (strcmp(command[i], "<") == 0) {
                if (i+1 == numOfCom) {
                    printf("Not input given\n");
                    return -1;
                }
                input_file = command[i+1];
            }
            else if (strcmp(command[i], ">") == 0) {
                if (i+1 == numOfCom) {
                    printf("Not output given\n");
                    return ;
                }
                output_file = command[i+1];
            }
            else if (strcmp(command[i], ">>") == 0) {
                if (i+1 == numOfCom) {
                    printf("Not output given\n");
                    return ;
                }
                append_file = command[i+1];
            }
            else {
                if (!(strcmp(command[i-1], ">") == 0 || strcmp(command[i-1], "<") == 0 || strcmp(command[i-1], ">>") == 0)) {
                    input_file = command[i];
                }
            }
        }

        if (input_file) {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            
            if (dup2(fd_in, STDIN_FILENO) == -1) {
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(fd_in);
        }
        
        if (output_file) {
            int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            
            if (dup2(fd_out, STDOUT_FILENO) == -1) {
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(fd_out);
        }

        if (append_file) {
            int fd_append = open(append_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd_append == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            
            if (dup2(fd_append, STDOUT_FILENO) == -1) {
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(fd_append);
        }

        execvp(command[0], NULL);
        exit(0);
    }
    wait(NULL);
}

int main (void) {
    
    
    int numOfCom = 0;
    bool is_redirection = false;
    bool is_piped= true;
    bool is_background = false;

    char** command = NULL;
    // char** command = malloc(MAX_ARGS * sizeof(char*));
    // for (int i=0; i < MAX_ARGS; i++)
    //     command[i] = malloc(MAX_SIZE_ARG* sizeof(char*));
    
    int ij = 0;
    while (ij < 3) {
        
        if (!readLine(&command, &numOfCom, &is_redirection, &is_piped, &is_background))
            return EXIT_FAILURE;
        
        if (strcmp(command[0], "reutno\n") == 0)
            break;
        
        printf("%d %d %d %d\n", numOfCom, is_redirection, is_piped, is_background);
        // for (int i = 0; i < numOfCom; i++)
        //     printf("%s\n", command[i]);
        
        if (!is_redirection && !is_piped && !is_background) {
            execSimple(command);
        }
        else if (is_redirection && !is_piped && !is_background) {
            execRedirection(command, numOfCom);
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



bool readLine(char*** command, int* numOfCom, bool* is_redirection, bool* is_piped, bool* is_background) {

    printf("in-mysh-now:>");

    // Read Line
    char* read;
    size_t len = 0;
    if (getline(&read, &len, stdin) == -1) {
        perror("getline");
        exit(EXIT_FAILURE);
    }

    *is_redirection = false;
    if (strstr(read, "<") || strstr(read, ">") || strstr(read, ">>") )
        *is_redirection = true;

    *is_piped = false;
    if (strstr(read, "|") )
        *is_piped = true;

    *is_background = false;
    if (strstr(read, "&") )
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


void execSimple(char** parsed) {
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
