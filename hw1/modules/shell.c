#include "../include/shell.h"
#include "../include/aliases.h"
#include "../include/background.h"
#include "../include/comands.h"
#include "../include/common.h"
#include "../include/history.h"
#include "../include/pipes.h"
#include "../include/redirection.h"
#include "../include/signals.h"
#include "../include/wildcharacters.h"

#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

// Simulation of a shell
void shell() {

    // Set signals not to have affect when on a shell
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);

    // Flags for the dieferences commands
    bool is_redirection, is_piped, is_background, has_wildcard, create_aliase, is_multi;

    char** command = NULL;
    int numOfCom;
    
    // Aliasse and value of them
    char** aliases = NULL;
    char** aliasesValue = NULL;
    int numAliases = 0;

    char* read = NULL;
    size_t len = 0;
    
    // Infinite loop Until press exit
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
        
        // History Update and changing the read
        historyUpdate(read);
        if (strstr(read, "myHistory ") || strstr(read, "myHistory") || strstr(read, "myHistory\n")) {
            if (!historyHandler(&read)) {
                continue;
            }
        }

        // Set flags and break the command in words
        comAliasesReplace(read, aliases, aliasesValue, numAliases);
        comInfo2(read,  &is_redirection, &is_piped, &is_background, &has_wildcard, &create_aliase, &is_multi);
        command = comBrk2(read, " ", &numOfCom);

        // Exit
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
        else if (is_multi) {
            multiComHandlere(command, numOfCom);
        }
        else if (is_background) {
            backgroundComAnalysis(command, numOfCom);
        }
        else if (is_piped) {
            pipeComAnalysis(command, numOfCom, false);
        }
        else if (create_aliase) {
           aliasesHandler(command, numOfCom, &aliases, &aliasesValue, &numAliases);
        }
        else if (has_wildcard) {
           wildcardsComAnalysis(command, numOfCom, false);
        }
        else if (is_redirection) {
            if (execRedirection(command, numOfCom, false)) {
                free(command);
                break;
            }
                
        }
        else {
            comSimpleExec(command, false);
        }

        free(command);

    }

    free(read);
    clear_history();

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
