#include "../include/pipes.h"
#include "../include/comands.h"
#include "../include/redirection.h"
#include "../include/wildcharacters.h"
#include "../include/history.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>


// The function first breaks the command into individual commands separated by the pipe character '|' 
// It then iterates over each command in the resulting array, creating a pipe and forking a child process for each one.
// In the child process, the input and output file descriptors are set up using dup2 to connect to the appropriate end of the pipe, 
// and the command is executed using comSimpleExec, wildcardsComAnalysis, or execRedirection, depending on the flags set by comInfo1.
// The function returns false if there are no errors and true otherwise.
bool pipeComAnalysis(char** command, int numOfCom, bool is_back) {
    int i;
    int pipefd[2];
    pid_t pid;
    int in = 0;
    int status;
    int pipNum = 0;
    char *** pipeCom= comBrk1(command, "|", &pipNum);
    bool is_redirection, is_piped, is_background, has_wildcard, create_aliase, is_history;

    for (i = 0; i < pipNum; i++) {
        if (pipe(pipefd) < 0) {
            perror("pipe");
            return true;
        }
        
        if ((pid = fork()) < 0) {
            perror("fork");
            return true;
        } 
        else if (pid == 0) {
            
            dup2(in, 0); 
            if (i < pipNum - 1) {
                dup2(pipefd[1], 1); 
            }
            close(pipefd[0]);

            int counter = countArgs(pipeCom[i]);
            comInfo1(pipeCom[i], counter, &is_redirection, &is_piped, &is_background, &has_wildcard, &create_aliase, &is_history);
            
            if (is_redirection) {
                execRedirection(pipeCom[i], counter, is_back);
            }
            else if (has_wildcard) {
                wildcardsComAnalysis(pipeCom[i], counter, is_back);
            }
            else {
                comSimpleExec(pipeCom[i], is_back);
            }
            exit(EXIT_FAILURE);
           
            
        } 
        else {
            waitpid(pid, &status, 0); 
            close(pipefd[1]); 
            in = pipefd[0]; 
        }
    }

    for (int i =0; pipeCom[i] != NULL; i++) {
        for (int j=0; pipeCom[i][j]!=NULL; j++) {
            free(pipeCom[i][j]);
        }
        free(pipeCom[i]);
    }
    free(pipeCom);

    return false;
}
