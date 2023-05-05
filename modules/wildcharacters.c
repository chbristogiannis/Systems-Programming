#include "../include/wildcharacters.h"

#include <dirent.h>

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

// This code is for wildcard command analysis. It takes an array of strings (command) and the number of commands (numOfCom) as input. 
// It creates a new array of strings (newCom) and scans each command in the input array. If a command contains a wildcard character (either '*' or '?'), it searches for all the files in the current directory that match the wildcard pattern using the match() function.
// It then adds all the matching files to the new command array. If a command does not contain a wildcard character, it simply copies the command to the new command array.
bool wildcardsComAnalysis(char **command, int numOfCom) {
    
    int newNumCom = 0;
    char **newCom = malloc(sizeof(char *) * (MAX_FILES));
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

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return true;
    } 
    else if (pid == 0) {
        if (execvp(newCom[0], newCom) < 0) {
            printf("\nCould not execute command..\n");
        }
        exit(0);
    } 

    int status;
    waitpid(pid, &status, 0);

    for (int i =0; i < newNumCom; i++) {
        free(newCom[i]);
    }
    free(newCom);

    return true;
}