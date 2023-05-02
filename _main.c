#include <stdio.h>

#include <string.h>
#include <stdbool.h>

#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>

#include <dirent.h>

#define MAX_ARGS 10
#define MAX_SIZE_ARG 100
#define MAX_FILES 10

bool readLine(char*** command, int* numOfCom, bool* is_redirection, bool* is_piped, bool* is_background, bool* has_wildcard, bool* create_aliases) ;
void execSimple(char** parsed);
bool execRedirection(char** command, int numOfCom);
void backgroundComAnalysis(char** command, int numOfCom) ;
int match(char *pattern, char *str);
void wildcardsComAnalysis(char **command, int numOfCom) ;

        // words = realloc(words, (i+1) * sizeof(char *));
        // words[i] = strdup(word); 

void aliasesHandler(char** command, int numOfCom, char ***aliases, char*** aliasesValue, int *numAliases) {
    if (strcmp(command[0], "createalias") == 0) {
        *aliases = realloc(*aliases, (*numAliases+1)*sizeof(char*));
        *aliasesValue = realloc(*aliases, (*numAliases+1)*sizeof(char*));
        *aliases[*numAliases] = strdup(command[1]);
        *aliases[*numAliases] = strdup(command[2]);
        (*numAliases)++;
    }
    else {
        for (int j=0; j < numOfCom; j++) {
            if (strcmp(command[0], *aliases[j]) == 0) {
                free(*aliases[j]);
                free(*aliasesValue[j]);
            }
        }
    }
}

int main (void) {
    
    int numOfCom = 0;
    bool is_redirection = false;
    bool is_piped= false;
    bool is_background = false;
    bool has_wildcard = false;
    bool create_aliase = false;

    char** command = NULL;
    
    char** aliases = NULL;
    char** aliases_value = NULL;
    int num_aliases = 0;
    // char** command = malloc(MAX_ARGS * sizeof(char*));
    // for (int i=0; i < MAX_ARGS; i++)
    //     command[i] = malloc(MAX_SIZE_ARG* sizeof(char*));
    
    while (true) {
        
        if (!readLine(&command, &numOfCom, &is_redirection, &is_piped, &is_background, &has_wildcard, &create_aliase))
            continue;
        
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

        printf("%d %d %d %d %d\n", numOfCom, is_redirection, is_piped, is_background, has_wildcard);
        // for (int i = 0; i < numOfCom; i++)
        //     printf("%s\n", command[i]);
        
        if (!is_redirection && !is_piped && !is_background && !has_wildcard) {
            execSimple(command);
        }
        else if (is_redirection && !is_piped && !is_background) {
            execRedirection(command, numOfCom);
        }
        else if (is_background) {
            backgroundComAnalysis(command, numOfCom);
        }
        else if (has_wildcard) {
           wildcardsComAnalysis(command, numOfCom);
        }
        else if (create_aliase) {
            aliasesHandler(command, numOfCom, &aliases, &aliases_value, &num_aliases);
        }

        for (int i = 0; i <numOfCom; i++) {
            free(command[i]);
        }
        free(command);
    }

    if (aliases != NULL) {
        for (int i=0; i < num_aliases; i ++){
            free(aliases[i]);
            free(aliases_value[i]);
        }
        free(aliases);
        free(aliases_value);
    }
    printf("Exiting ...\n");
    // for (int i = 0; i < MAX_ARGS; i++) {
    //     free(command[i]);
    // }
    // free(command);
    
    return 0;
}



bool readLine(char*** command, int* numOfCom, bool* is_redirection, bool* is_piped, bool* is_background, bool* has_wildcard, bool* create_aliases) {

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
    
    *has_wildcard = false;
    if (strstr(read, "*") || strstr(read, "?"))
        *has_wildcard = true;
    
    *create_aliases = false;
    if (strstr(read, "createalias") || strstr(read, "destroyalias"))
        *create_aliases = true;

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


int match(char *pattern, char *str) {
    if (*pattern == '\0' && *str == '\0') {
        return 1;
    }
    if (*pattern == '*' && *(pattern + 1) != '\0' && *str == '\0') {
        return 0;
    }
    if (*pattern == '?' || *pattern == *str) {
        return match(pattern + 1, str + 1);
    }
    if (*pattern == '*') {
        return match(pattern + 1, str) || match(pattern, str + 1);
    }
    return 0;
}

void wildcardsComAnalysis(char **command, int numOfCom) {
    
    int newNumCom = 0;
    char **newCom = malloc(sizeof(char *) * (MAX_FILES+numOfCom));
    for (int i = 0; i < numOfCom; i++) {
        
        if (strchr(command[i], '*') != NULL || strchr(command[i], '?') != NULL) {
            
            DIR *dir;
            struct dirent *entry;
            dir = opendir(".");
            if (dir != NULL) {
                
                while ((entry = readdir(dir)) != NULL) {
                    if (match(command[i], entry->d_name)) {
                        newCom[newNumCom++] = strdup(entry->d_name);
                    }
                }
                closedir(dir);
            }
        }
        else {
            newCom[newNumCom++] = strdup(command[i]);
        }
    }
    // printf("0000%d\n", newNumCom);
    // for (int i =0; i<newNumCom; i++) {
    //     printf("%s\n", newCom[i]);
    // }
    // for (int i = 0; i < newNumCom; i++) {
    //     command[num_newCom + i] = command[i];
    //     command[i] = newCom[i];
    // }
    // command[newNumCom + 1] = NULL;
    // for (int i=0; command[i] != NULL; i ++) {
    //     printf("%s ", command[i]);
    // }
    // free(newCom);
    execSimple(newCom);
    for (int i =0; i < newNumCom; i++) {
        free(newCom[i]);
    }
    free(newCom);
}