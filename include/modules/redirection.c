#include "../include/redirection.h"
#include "../include/comands.h"

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// The function first saves the current standard input and output file descriptors using the dup function.
// It then iterates through the command array, searching for the "<", ">", and ">>" symbols that indicate input and output redirection. 
// If it finds one of these symbols, it opens the appropriate file using the open function, and redirects the standard input or output file descriptor using the dup2 function.
// After the input and output have been redirected, the function calls comSimpleExec differences in case is called from backgorund
// The function returns a boolean value depending on error.
bool execRedirection(char** command, int numOfCom, bool is_back) {
    int in = STDIN_FILENO;
    int out = STDOUT_FILENO;
    int orig_in = dup(in);
    int orig_out = dup(out);

    for (int i = 0; command[i] != NULL; i++) {
        if (strcmp(command[i], "<") == 0) {
            if (i == numOfCom - 1) {
                printf("Missing input redirection file\n");
                return true;
            }
            in = open(command[i + 1], O_RDONLY);
            if (in < 0) {
                perror("open\n");
                return true;
            }
            dup2(in, STDIN_FILENO);
            close(in);
            command[i] = NULL;
            i++;
        }
        else if (strcmp(command[i], ">") == 0) {
            if (i == numOfCom - 1) {
                perror("Missing output redirection file\n");
                return true;
            }
            out = open(command[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out < 0) {
                perror("open");
                return true;
            }
            dup2(out, STDOUT_FILENO);
            close(out);
            command[i] = NULL;
            i++;
        }
        else if (strcmp(command[i], ">>") == 0) {
            if (i == numOfCom - 1) {
                printf("Missing output redirection file\n");
                return true;
            }
            out = open(command[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (out < 0) {
                perror("open");
                return true;
            }
            dup2(out, STDOUT_FILENO);
            close(out);
            command[i] = NULL;
            i++;
        }
    }

    comSimpleExec(command, is_back);

    // reset standard input and output file descriptors
    dup2(orig_in, STDIN_FILENO);
    dup2(orig_out, STDOUT_FILENO);
    close(orig_in);
    close(orig_out);

    return false;
}