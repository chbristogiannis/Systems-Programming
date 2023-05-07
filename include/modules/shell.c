#include "../include/shell.h"
#include "../include/history.h"
#include "../include/comands.h"
#include "../include/aliases.h"
#include "../include/background.h"
#include "../include/pipes.h"
// #include <stdio.h>
#include <string.h>
// #include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>




void shell() {

    bool is_redirection, is_piped, is_background, has_wildcard, create_aliase, is_history;

    char** command = NULL;
    int numOfCom;
    
    char** aliases = NULL;
    char** aliasesValue = NULL;
    int numAliases = 0;

    char* read = NULL;
    size_t len = 0;
    while (true) {
        printf("\nin-mysh-now:>");
        if (getline(&read, &len, stdin) == -1) {
            perror("Getline failed");
            exit(EXIT_FAILURE);
        }
        size_t len = strlen(read);
        if (len > 0 && read[len-1] == '\n') {
            read[len-1] = '\0';
            len--;
        }
        
        if (read == NULL)
            continue;

        if (strstr(read, "myHistory ") || strstr(read, "myHistory") || strstr(read, "myHistory\n")) {
            if (!historyHandler(&read)) {
                continue;
            }
        }

        comAliasesReplace(read, aliases, aliasesValue, numAliases);
        comInfo2(read,  &is_redirection, &is_piped, &is_background, &has_wildcard, &create_aliase, &is_history);
        command = comBrk2(read, " ", &numOfCom);
        

        printf("%d %d %d %d %d %d %d\n", numOfCom, is_redirection, is_piped, is_background, has_wildcard, create_aliase, is_history);
        for (int i = 0; command[i] != NULL; i++)
            printf("%s\n", command[i]);

        if (strcmp(command[0], "exit") == 0) {
            free(command);
            break;
        }

        if (strcmp(command[0], "cd") == 0) {
            if (numOfCom == 1) {
                chdir(getenv("HOME"));
            }
            else {
                chdir(command[1]);
            }
            continue;
        }

        if (is_background) {
            backgroundComAnalysis(command, numOfCom);
        }
        else if (is_piped) {
            pipeComAnalysis(command, numOfCom, false);
        }
        else if (create_aliase) {
           if (aliasesHandler(command, numOfCom, &aliases, &aliasesValue, &numAliases));
        }
        else if (has_wildcard) {
           if (wildcardsComAnalysis(command, numOfCom, false));
        }
        else if (is_redirection) {
            if (execRedirection(command, numOfCom, false));
        }
        else {
            comSimpleExec(command, false);
        }

        historyUpdate(read);
        free(command);

    }

    free(read);

    for (int i=0; i < numAliases; i ++){
        free(aliases[i]);
        free(aliasesValue[i]);
    }
    if (aliases != NULL)
        free(aliases);
    if (aliasesValue != NULL)
        free(aliasesValue);

    


    printf("Exiting ...\n");
}
