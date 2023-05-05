#include "../include/pipes.h"

bool pipeComAnalysis(char** command, int numOfCom) {
    
    char** pipCom = (char**) malloc(MAX_PIPES * sizeof(char*));
    int pipNum = 0;

    // Copy elements until "&" character is read
    for (int i = 0; i < numOfCom; i++) {
        if (strcmp(command[i], "|") == 0) {
            
        }
        else {

        }

    }
    return false;
}