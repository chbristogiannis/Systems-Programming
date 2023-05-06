#include "../include/aliases.h"

// It creates and destoyes alias
// if the command is to createalias takes the next word and saves it to aliases and all the next ones to aliasesValue
// if the command is to destroy a specific alias it finds it and frees it
bool aliasesHandler(char** command, int numOfCom, char ***aliases, char*** aliasesValue, int *numAliases) {
    for (int i =0; i < numOfCom; i ++) {
        if (strcmp(command[i], "createalias") == 0) {
            if (i+2 >= numOfCom) {
                printf("Not enough arguments");
                return true;
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
        }
        else if (strcmp(command[i], "destroyalias") == 0){
            if (i+1 >= numOfCom) {
                printf("Not enough arguments");
                return true;
            }
            for (int j=0; j < *numAliases; j++) {
                int len1 = strlen(command[i+1]);
                if (strcmp(command[i+1], (*aliases)[j]) == 0 || (strncmp(command[i+1],(*aliases)[j], len1-1) == 0 && strlen((*aliases)[j]) == len1-1)) {
                    free((*aliases)[j]);
                    free((*aliasesValue)[j]);
                    (*numAliases)--;
                }
            }
        }
    }

    return false;
}
