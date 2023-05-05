#include "../include/background.h"


bool backgroundComAnalysis(char** command, int numOfCom) {
    
    char** backCom = (char**) malloc(MAX_BACKGROUND * sizeof(char*));
    int backNum = 0;

    // Copy elements until "&" character is read
    for (int i = 0; i < numOfCom; i++) {
        if (strcmp(command[i], "&") == 0) {
            
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                return true;
            } else if (pid == 0) {
                if (execvp(backCom[0], backCom) < 0) {
                    printf("\nCould not execute command..\n");
                }
                exit(0);
            }
            for (int j=0; j < backNum; j++) {
                free(backCom[j]);
            }
            backNum = 0;
        }
        else {
            backCom[backNum] = (char*) malloc(strlen(command[i]) + 1);
            strcpy(backCom[backNum], command[i]);
            backNum++;
        }

    }
    free(backCom);
    return false;
}