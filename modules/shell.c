#include "../include/shell.h"
#include "../include/history.h"

// #include <stdio.h>
#include <string.h>
// #include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>


// the readLine function takes user input from the standard input and tokenizes it into individual words.
// updates the history (not myHistory commands)
// checking for various flags such as redirection, piping, background execution, and wildcard expansion. 
// It also checks for the createalias commands.
bool readLine(char*** command, int* numOfCom, char** aliases, char** aliasesValue, int numAliases, bool* is_redirection, bool* is_piped, bool* is_background, bool* has_wildcard, bool* create_aliases) {

    // Read line command incase it is history releated it either shows the history and repeat itself or
    // changes the read with the one that the user ask, or it saves the command and continues
    char* read = NULL;
    size_t len = 0;
    while(true) {
        printf("\nin-mysh-now:>");
        if (getline(&read, &len, stdin) == -1) {
            perror("Getline failed");
            return true;
        }

        if (strstr(read, "myHistory ") || strstr(read, "myHistory\n")) {
            if (historyHandler(&read)) {
                break;
            }  
        }
        else {
            update_history(read);
            break;
        }
    }
    // Flag part that gives info about what fuctions is needed
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

    // tokenizes the input string, checks every word if it aliase and replace it if necessary
    char *word = strtok(read, " ");
    char *alword = NULL;
    bool flag_changed = false;
    while (word != NULL && strcmp(word, "\n") != 0) {
        for (int j=0; j != numAliases; j++) {
            int len1 = strlen(word);
            if (strcmp(word, aliases[j]) == 0 || (strncmp(word, aliases[j], len1-1) == 0 && strlen(aliases[j]) == len1-1)) {
                alword = strtok(aliasesValue[j], " ");
                while (alword != NULL && strcmp(alword, "\n") != 0) {
                    *command = realloc(*command, (*numOfCom+1) * sizeof(char *));
                    (*command)[*numOfCom] = strdup(alword);
                    (*numOfCom)++;
                    alword = strtok(NULL, " ");
                }
                flag_changed = true;
            }
        }
        if (!flag_changed) {
            *command = realloc(*command, (*numOfCom+1) * sizeof(char *));
            (*command)[*numOfCom] = strdup(word);
            (*numOfCom)++;
        }
        flag_changed = false;        
        word = strtok(NULL, " ");
    }

    // removes \n from last character
    if (strstr((*command)[*numOfCom-1], "\n")) {
        (*command)[*numOfCom-1][strlen((*command)[*numOfCom-1])-1] = '\0';
    }
    return false;
}


void shell() {

    bool is_redirection, is_piped, is_background, has_wildcard, create_aliase;

    char** command = NULL;
    int numOfCom;
    
    char** aliases = NULL;
    char** aliasesValue = NULL;
    int numAliases = 0;

    while (true) {
        numOfCom = 0;
        if (readLine(&command, &numOfCom, aliases, aliasesValue, numAliases, &is_redirection, &is_piped, &is_background, &has_wildcard, &create_aliase))
            break;
        
        if (strcmp(command[0], "exit") == 0) 
            break;
        
        if (strcmp(command[0], "cd") == 0) {
            if (numOfCom == 1) {
                chdir(getenv("HOME"));
            }
            else {
                chdir(command[1]);
            }
            continue;
        }

        printf("%d %d %d %d %d %d\n", numOfCom, is_redirection, is_piped, is_background, has_wildcard, create_aliase);
        for (int i = 0; i < numOfCom; i++)
            printf("%s\n", command[i]);
        
        if (!is_redirection && !is_piped && !is_background && !has_wildcard && !create_aliase) {
            // execSimple(command);
        }
        else if (is_redirection) {
            if (execRedirection(command, numOfCom));
        }
        else if (is_background) {
            backgroundComAnalysis(command, numOfCom);
        }
        else if (has_wildcard) {
           if (wildcardsComAnalysis(command, numOfCom));
        }
        else if (create_aliase) {
           if (aliasesHandler(command, numOfCom, &aliases, &aliasesValue, &numAliases));
        }


        for (int i = 0; i <numOfCom; i++) {
            free(command[i]);
        }
        free(command);
    }


    for (int i=0; i < numAliases; i ++){
        free(aliases[i]);
        free(aliasesValue[i]);
    }
    free(aliases);
    free(aliasesValue);

    printf("Exiting ...\n");
}
