#include "../include/aliases.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// The fuction is responsible for creating and deleting alias based on the given command
// First finds what the command wants from the 2 options
// If it wants to create an alias checks if alias exist with the same name and 
// if the arguments are enouch to complete the procedure
// Then add the next word to alias and the all the next ones to aliasesValue and adds to the size of them
// If the command is to delete an alias finds it and delete it and the aliasesValue and decreases the size
void aliasesHandler(char** command, int numOfCom, char ***aliases, char*** aliasesValue, int *numAliases) {
    for (int i =0; i < numOfCom; i ++) {
        if (strcmp(command[i], "createalias") == 0) {
            for (int j=0; j < *numAliases; j++) {
                int len1 = strlen(command[i+1]);
                if (strcmp(command[i+1], (*aliases)[j]) == 0 || (strncmp(command[i+1],(*aliases)[j], len1-1) == 0 && strlen((*aliases)[j]) == len1-1)) {
                    printf("There is already a alias with this name ... continue\n");
                    return ;
                }
            }
            
            if (i+2 >= numOfCom) {
                printf("Not enough arguments");
                return ;
            }
            *aliases = realloc(*aliases, (*numAliases+1)*sizeof(char*));
            *aliasesValue = realloc(*aliasesValue, (*numAliases+1)*sizeof(char*));
            
            int numStr = numOfCom;
            int total_len = 0;
            for (int j = i+2; j < numStr; j++) {
                total_len += strlen(command[j]) + 1; 
            }
            
            (*aliasesValue)[*numAliases] = (char*)malloc(total_len + 1);
            (*aliasesValue)[*numAliases][0] = '\0';
            for (int j = i+2; j < numStr; j++) {
                strcat((*aliasesValue)[*numAliases], command[j]);
                if (j < numStr - 1) {
                    strcat((*aliasesValue)[*numAliases], " ");
                }
            }            
            (*aliases)[*numAliases] = strdup(command[i+1]);
            (*numAliases)++;
            return;
        }
        else if (strcmp(command[i], "destroyalias") == 0){
            if (i+1 >= numOfCom) {
                printf("Not enough arguments");
                return;
            }
            for (int j=0; j < *numAliases; j++) {
                int len1 = strlen(command[i+1]);
                if (strcmp(command[i+1], (*aliases)[j]) == 0 || (strncmp(command[i+1],(*aliases)[j], len1-1) == 0 && strlen((*aliases)[j]) == len1-1)) {
                    free((*aliases)[j]);
                    free((*aliasesValue)[j]);
                    (*numAliases)--;
                }
            }
            return;
        }
    }
}
