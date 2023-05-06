#include "../include/pipes.h"
#include "../include/comands.h"
#include "../include/redirection.h"
#include "../include/wildcharacters.h"
#include "../include/history.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


bool pipeComAnalysis(char** command, int numOfCom) {
    int i;
    int pipefd[2];
    pid_t pid;
    int in = 0;
    int status;
    int pipNum = 0;
    char *** pipeCom= comBrk1(command, "|", &pipNum);

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
//             int counter = 0;
//             for (counter; pipeCom[i][counter] != NULL; counter++) {
//                 printf("%s\n", pipeCom[i][counter]);
//             }
//             printf("%d %d %d %d %d %d\n", counter, is_redirection, is_piped, is_background, has_wildcard, create_aliase);
            
//             if (is_redirection) {
//                 execRedirection(pipeCom[i], counter);
//             }
//             else if (has_wildcard) {
//                 wildcardsComAnalysis(pipeCom[i], counter);
//             }
//             else if (is_history) {
//                 historyHandler(pipeCom[i]);
//             }
//             else {
//                 comSimpleExec(pipeCom[i]);
//             }
           
            
        } 
        else {
            
            waitpid(pid, &status, 0); 
            close(pipefd[1]); 
            in = pipefd[0]; 
        }
    }
}






// bool pipeComAnalysis(char** command, int numOfCom) {


//     int pipeNum = 0;
//     printf("PIPE FROM HERE ON\n");
    
//     // for (int i = 0; command[i] != NULL; i++)
//     //     printf("%s\n", command[i]);

//     char*** pipeCom = NULL;
//     pipeCom = comBrk1(command, "|", &pipeNum);
//     // for (int i=0; i < pipeNum; i++) {
//     //     printf("%s\n", pipeCom[i][0]);
//     // }


//     bool is_redirection, is_piped, is_background, has_wildcard, create_aliase, is_history;

//     int pipefd[2];
//     pid_t pid;
//     int in = 0;
//     int status;
//     for (int i =0; i < pipeNum; i++) { 
//         if (pipe(pipefd) < 0) {
//             perror("pipe");
//             exit(1);
//         }
        
//         if ((pid = fork()) < 0) {
//             perror("fork");
//             exit(1);
//         }
//         else if (pid == 0) {

//             dup2(in, 0); /* connect stdin to the previous pipe's output */
//             if (i < pipeNum - 1) {
//                 dup2(pipefd[1], 1); /* connect stdout to the next pipe's input */
//             }
//             close(pipefd[0]); /* close unused read end of pipe */

//             comInfo1(pipeCom[i], sizeof(pipeCom[i])/sizeof(char*), &is_redirection, &is_piped, &is_background, &has_wildcard, &create_aliase, &is_history);
//             int counter = 0;
//             for (counter; pipeCom[i][counter] != NULL; counter++) {
//                 printf("%s\n", pipeCom[i][counter]);
//             }
//             printf("%d %d %d %d %d %d\n", counter, is_redirection, is_piped, is_background, has_wildcard, create_aliase);
            
//             if (is_redirection) {
//                 execRedirection(pipeCom[i], counter);
//             }
//             else if (has_wildcard) {
//                 wildcardsComAnalysis(pipeCom[i], counter);
//             }
//             else if (is_history) {
//                 historyHandler(pipeCom[i]);
//             }
//             else {
//                 comSimpleExec(pipeCom[i]);
//             }
//         } 
//         else {
//             waitpid(pid, &status, 0); /* wait for child process to complete */
//             close(pipefd[1]); /* close unused write end of pipe */
//             in = pipefd[0]; /**/
//         }        
//     }


//     for (int i =0; pipeCom[i] != NULL; i++) {
//         for (int j=0; pipeCom[i][j]!=NULL; j++) {
//             free(pipeCom[i][j]);
//         }
//         free(pipeCom[i]);
//     }
//     free(pipeCom);

//     return true;
// }
