#include "../include/pipes.h"
#include "../include/comands.h"
#include "../include/redirection.h"
#include "../include/wildcharacters.h"
#include "../include/history.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
            exit(1);
        }
        
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } 
        else if (pid == 0) {
            /* child process */
            dup2(in, 0); 
            if (i < pipNum - 1) {
                dup2(pipefd[1], 1); 
            }
            close(pipefd[0]);
            int counter = 0;
            for (counter; pipeCom[i][counter] != NULL; counter++) {
                // printf("%s\n", pipeCom[i][counter]);
            }
            comInfo1(pipeCom[i], counter, &is_redirection, &is_piped, &is_background, &has_wildcard, &create_aliase, &is_history);
            // printf("%d %d %d %d %d %d\n", counter, is_redirection, is_piped, is_background, has_wildcard, create_aliase);
            
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

    return true;
}
