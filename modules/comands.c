#include "../include/shell.h"
#include "../include/aliases.h"
#include "../include/background.h"
#include "../include/comands.h"
#include "../include/common.h"
#include "../include/history.h"
#include "../include/pipes.h"
#include "../include/redirection.h"
#include "../include/signal.h"
#include "../include/wildcharacters.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <readline/history.h>
#include <sys/wait.h>

// The function initializes all boolean pointers to false. Then, it loops through each element of the command array and checks for certain strings to
// determine whether the given command contains 
// redirection, piping, background processes, wildcard characters, or alias creation/destruction multiple commands.
void comInfo1(char** command, int numOfCom, bool* is_redirection, bool* is_piped, bool* is_background, bool* has_wildcard, bool* create_aliases, bool* is_multi) {
   *is_multi = false;
   *is_redirection = false;
   *is_piped = false;
   *is_background = false;
   *has_wildcard = false;
   *create_aliases = false;
    for (int i=0; command[i] != NULL; i++) {
        
        if (strstr(command[i], " ; ") || strstr(command[i], ";\n")) 
            *is_multi = true;
        
        if (strstr(command[i], "<") || strstr(command[i], ">") || strstr(command[i], ">>") )
            *is_redirection = true;
        
        if (strstr(command[i], "|") )
            *is_piped = true;
        
        if (strstr(command[i], "&") )
            *is_background = true;
                
        if (strstr(command[i], "*") || strstr(command[i], "?"))
            *has_wildcard = true;
                
        if (strstr(command[i], "createalias") || strstr(command[i], "destroyalias"))
            *create_aliases = true;        
    }
    
}

// The function initializes all boolean pointers to false. Then, it loops through each element of the command array and checks for certain strings to
// determine whether the given command contains 
// redirection, piping, background processes, wildcard characters, or alias creation/destruction multiple commands.
void comInfo2(char* command, bool* is_redirection, bool* is_piped, bool* is_background, bool* has_wildcard, bool* create_aliases, bool* is_multi) {

    *is_multi = false;
    if (strstr(command, " ; ") || strstr(command, ";\n")) 
       *is_multi = true;

    *is_redirection = false;
    if (strstr(command, "<") || strstr(command, ">") || strstr(command, ">>") )
        *is_redirection = true;

    *is_piped = false;
    if (strstr(command, "|") )
        *is_piped = true;

    *is_background = false;
    if (strstr(command, "&") )
        *is_background = true;
    
    *has_wildcard = false;
    if (strstr(command, "*") || strstr(command, "?"))
        *has_wildcard = true;
    
    *create_aliases = false;
    if (strstr(command, "createalias") || strstr(command, "destroyalias"))
        *create_aliases = true;
}


// This function is used to break a command into individual commands by splitting it at a specific element (remove). 
// It creates a two-dimensional array of strings, where each row represents a command and each column represents an argument. 
// The comNum parameter is used to store the number of commands that were created.
char*** comBrk1(char** command, char* remove, int* comNum) {
    
    char*** commands = (char***) malloc(MAX_COMMANDS * sizeof(char**));
    for (int i = 0; i < MAX_COMMANDS; i++) {
        commands[i] = (char**) malloc(MAX_ARGUMENTS * sizeof(char*));
    }

    int cmd_idx = 0;
    int arg_idx = 0;

    for (int i = 0; command[i] != NULL; i++) {
        if (strcmp(command[i], remove) == 0) {
            commands[cmd_idx][arg_idx] = NULL;
            cmd_idx++;
            arg_idx = 0;
        } 
        else {
            commands[cmd_idx][arg_idx] = strdup(command[i]);
            arg_idx++;
        }
    }

    commands[cmd_idx][arg_idx] = NULL;
    commands[cmd_idx+1] = NULL;
    *comNum = cmd_idx+1;

    return commands;
}

// Τhis function is used to break a command into individual commands by splitting it at a specific character (remove). 
// It creates an array of strings, where each element represents a command. 
// The comNum parameter is used to store the number of commands that were created.
char** comBrk2(char* command, char* remove, int* comNum) {
    // Trim leading whitespace
    while (*command == ' ')
        command++;
    
    // Trim trailing whitespace
    int len = strlen(command);
    while (len > 0 && command[len - 1] == ' ') {
        command[--len] = '\0';
    }

    int count = 1;
    for (int i = 0; command[i]; i++) {
        if (command[i] == *remove) {
            count++;
        }
    }

    char** result = malloc((count + 1) * sizeof(char*));
    if (result == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    char* token = command;
    while (*command) {
        if (*command == *remove) {
            *command = '\0';
            if (strlen(token) > 0) {
                result[i++] = token;
            }
            token = command + 1;
        }
        command++;
    }
    if (strlen(token) > 0) {
        result[i++] = token;
    }

    result[i] = NULL;
    *comNum = i;

    return result;
}

// Τhis function is used to execute a simple command by forking a new process and running the command in the child process using the execvp() system call. 
// The is_back parameter is used to determine whether the function should wait for the command to finish executing before returning.
void comSimpleExec(char** command, bool is_back) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) {

        if (execvp(command[0], command) < 0) {
            printf("\nCould not execute command..\n");
        }
        exit(0);
    } 

    if (!is_back) {

        int status;
        waitpid(pid, &status, 0);
    }
}

// Add space infront and back from a char useful for alias
char* addSpaces(const char* str) {
    size_t len = strlen(str);
    char* newStr = malloc(len + 3);
    if (newStr == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    newStr[0] = ' ';
    memcpy(newStr + 1, str, len);
    newStr[len + 1] = ' ';
    newStr[len + 2] = '\0';
    return newStr;
}


void comAliasesReplace(char* command,  char** aliases, char** aliasesV, int aliasesNum) {

    for (int i=0; i < aliasesNum; i++) {
        char* temp = addSpaces(aliases[i]);
        char* pos = strstr(command, temp);
        if (pos != NULL) {
            size_t oldLen = strlen(temp);
            char* newTemp = addSpaces(aliasesV[i]);
            size_t newLen = strlen(newTemp);
            memmove(pos + newLen, pos + oldLen, strlen(pos + oldLen) + 1);
            memcpy(pos, newTemp, newLen);     
            free(newTemp);  
        }
        free(temp);
    }
}
// The purpose of adding spaces around the alias name and value is to ensure that the replacement 
// only occurs when the alias is used as a separate word, not as part of a longer word.


// The function starts by breaking up the command list into separate commands using the comBrk1() function. 
// Then it goes through each command in the list and analyzes it for certain features, 
// such as whether it contains a pipe (is_piped), a redirection (is_redirection), or a wildcard (has_wildcard).
// and we execute them one after the other
void multiComHandlere(char** commands, int numCom) {
    
    int numMCom;
    char*** multCom = comBrk1(commands, ";",& numMCom);

    bool is_redirection, is_piped, is_background, has_wildcard, create_aliase, is_multi;
    
    for (int i =0; multCom[i] != NULL; i++) { 
        int counter = countArgs(multCom[i]);
        comInfo1(multCom[i], counter, &is_redirection, &is_piped, &is_background, &has_wildcard, &create_aliase, &is_multi);
        
        if (is_background) {
            backgroundComAnalysis(multCom[i], counter);
        }
        else if (is_piped) {
            pipeComAnalysis(multCom[i], counter, false);
        }
        else if (has_wildcard) {
           wildcardsComAnalysis(multCom[i], counter, false);
        }
        else if (is_redirection) {
            if (execRedirection(multCom[i], counter, false));
        }
        else {
            comSimpleExec(multCom[i], false);
        }
    }


    for (int i =0; multCom[i] != NULL; i++) {
        for (int j=0; multCom[i][j]!=NULL; j++) {
            free(multCom[i][j]);
        }
        free(multCom[i]);
    }
    free(multCom);

}


int countArgs(char **command) {
    int count = 0;
    while (command[count] != NULL) {
        count++;
    }
    return count;
}