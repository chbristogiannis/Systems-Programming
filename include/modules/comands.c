#include "../include/comands.h"
#include "../include/signals.h"
#include <signal.h>
#include <sys/wait.h>


void comInfo1(char** command, int numOfCom, bool* is_redirection, bool* is_piped, bool* is_background, bool* has_wildcard, bool* create_aliases, bool* is_history) {
   *is_history = false;
   *is_redirection = false;
   *is_piped = false;
   *is_background = false;
   *has_wildcard = false;
   *create_aliases = false;
    for (int i=0; command[i] != NULL; i++) {
        
        if (strstr(command[i], "myHistory ") || strstr(command[i], "myHistory\n")) 
            *is_history = true;
        
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

void comInfo2(char* command, bool* is_redirection, bool* is_piped, bool* is_background, bool* has_wildcard, bool* create_aliases, bool* is_history) {

    *is_history = false;
    if (strstr(command, "myHistory ") || strstr(command, "myHistory")) 
       *is_history = true;

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

char** comBrk2(char* command, char* remove, int* comNum) {
    // Trim leading whitespace
    while (*command == ' ') {
        command++;
    }

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

        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);

        int status;
        waitpid(pid, &status, 0);
    }
}


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
        // printf("command is %s and aliases is %s\n", command, temp);
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
    // printf("command is %s \n", command);
}
