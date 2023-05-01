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
bool execRedirection(char** command, int numOfCom);
void backgroundComAnalysis(char** command, int numOfCom) ;

int main (void) {
    
    int numOfCom = 0;
    bool is_redirection = false;
    bool is_piped= true;
    bool is_background = false;

    char** command = NULL;
    // char** command = malloc(MAX_ARGS * sizeof(char*));
    // for (int i=0; i < MAX_ARGS; i++)
    //     command[i] = malloc(MAX_SIZE_ARG* sizeof(char*));
    
    while (true) {
        
        if (!readLine(&command, &numOfCom, &is_redirection, &is_piped, &is_background))
            continue;;
        
        if (strcmp(command[0], "reutno\n") == 0)
            break;
        
        if (strcmp(command[0], "exit") == 0) {
            break;
        }
        else if (strcmp(command[0], "cd") == 0) {
            if (numOfCom == 1) {
                chdir(getenv("HOME"));
            }
            else {
                chdir(command[1]);
            }
            continue;
        }

        // printf("%d %d %d %d\n", numOfCom, is_redirection, is_piped, is_background);
        // for (int i = 0; i < numOfCom; i++)
        //     printf("%s\n", command[i]);
        
        if (!is_redirection && !is_piped && !is_background) {
            execSimple(command);
        }
        else if (is_redirection && !is_piped && !is_background) {
            execRedirection(command, numOfCom);
        }
        else if (is_background) {
            backgroundComAnalysis(command, numOfCom);
        }

        for (int i = 0; i <numOfCom; i++) {
            free(command[i]);
        }
        free(command);
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
        perror("fork");
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command..\n");
        }
        exit(0);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

bool execRedirection(char** command, int numOfCom) {
    int in = STDIN_FILENO, out = STDOUT_FILENO;
    int orig_in = dup(in), orig_out = dup(out); // store original file descriptors
    for (int i = 0; i < numOfCom; i++) {
        if (strcmp(command[i], "<") == 0) {
            if (i == numOfCom - 1) {
                fprintf(stderr, "Missing input redirection file\n");
                return false;
            }
            in = open(command[i + 1], O_RDONLY);
            if (in < 0) {
                perror("open");
                return false;
            }
            dup2(in, STDIN_FILENO);
            close(in);
            command[i] = NULL;
            i++;
        }
        else if (strcmp(command[i], ">") == 0) {
            if (i == numOfCom - 1) {
                fprintf(stderr, "Missing output redirection file\n");
                return false;
            }
            out = open(command[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out < 0) {
                perror("open");
                return false;
            }
            dup2(out, STDOUT_FILENO);
            close(out);
            command[i] = NULL;
            i++;
        }
        else if (strcmp(command[i], ">>") == 0) {
            if (i == numOfCom - 1) {
                fprintf(stderr, "Missing output redirection file\n");
                return false;
            }
            out = open(command[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (out < 0) {
                perror("open");
                return false;
            }
            dup2(out, STDOUT_FILENO);
            close(out);
            command[i] = NULL;
            i++;
        }
    }

    execSimple(command);

    // reset standard input and output file descriptors
    dup2(orig_in, STDIN_FILENO);
    dup2(orig_out, STDOUT_FILENO);
    close(orig_in);
    close(orig_out);

    return true;
}

void backgroundComAnalysis(char** command, int numOfCom) {
    
    char** background_command = (char**) malloc(MAX_ARGS * sizeof(char*));
    int background_count = 0;

    // Copy elements until "&" character is read
    for (int i = 0; i < numOfCom; i++) {
        if (strcmp(command[i], "&") == 0) {
            for (int j=0; j < background_count; j++) {
                printf("%s\n", background_command[j]);
            }
            background_command[background_count] = NULL;
            
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                return;
            } else if (pid == 0) {
                if (execvp(background_command[0], background_command) < 0) {
                    printf("\nCould not execute command..\n");
                }
                exit(0);
            }
            for (int j=0; j < background_count; j++) {
                free(background_command[j]);
            }
            background_count = 0;
        }
        else {
            background_command[background_count] = (char*) malloc(strlen(command[i]) + 1);
            strcpy(background_command[background_count], command[i]);
            background_count++;
        }

    }
}