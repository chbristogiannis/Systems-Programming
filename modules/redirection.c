#include "../include/redirection.h"

#include <unistd.h>

#include <fcntl.h>

// for wait
// #include <sys/wait.h>
// #include <sys/types.h>

bool execRedirection(char** command, int numOfCom) {
    int in = STDIN_FILENO;
    int out = STDOUT_FILENO;
    int orig_in = dup(in);
    int orig_out = dup(out);

    for (int i = 0; command[i] != NULL; i++) {
        if (strcmp(command[i], "<") == 0) {
            if (i == numOfCom - 1) {
                perror("Missing input redirection file\n");
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
                perror("Missing output redirection file\n");
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

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return true;
    } 
    else if (pid == 0) {
        if (execvp(command[0], command) < 0) {
            printf("\nCould not execute command..\n");
        }
        exit(0);
    } 

    int status;
    waitpid(pid, &status, 0);
    

    // reset standard input and output file descriptors
    dup2(orig_in, STDIN_FILENO);
    dup2(orig_out, STDOUT_FILENO);
    close(orig_in);
    close(orig_out);

    return false;
}