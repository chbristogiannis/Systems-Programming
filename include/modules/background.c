#include "../include/background.h"
#include "../include/comands.h"

bool backgroundComAnalysis(char** command, int numOfCom) {
    
    // printf("HERE\n");
    int backNum = 0;
    
    char*** backCom = comBrk1(command, "&", &backNum);
    // for (int i=0; i < backNum; i++) {
    //     printf("%s\n", backCom[i][0]);
    // }
    bool is_redirection, is_piped, is_background, has_wildcard, create_aliase, is_history;

    for (int i =0; backCom[i] != NULL; i++) { 
        comInfo1(backCom[i], 0, &is_redirection, &is_piped, &is_background, &has_wildcard, &create_aliase, &is_history);
        if (is_piped) {
            pipeComAnalysis(backCom[i], numOfCom, true);
        }
        else if (has_wildcard) {
           if (wildcardsComAnalysis(backCom[i], numOfCom, true));
        }
        else if (is_redirection) {
            if (execRedirection(backCom[i], numOfCom, true));
        }
        else {
            comSimpleExec(backCom[i], true);
        }

    }


    for (int i =0; backCom[i] != NULL; i++) {
        for (int j=0; backCom[i][j]!=NULL; j++) {
            free(backCom[i][j]);
        }
        free(backCom[i]);
    }
    free(backCom);


    // char** backCom = (char**) malloc(MAX_BACKGROUND * sizeof(char*));
    // int backNum = 0;

    // // Copy elements until "&" character is read
    // for (int i = 0; i < numOfCom; i++) {
    //     if (strcmp(command[i], "&") == 0) {
            
    //         pid_t pid = fork();
    //         if (pid == -1) {
    //             perror("fork");
    //             return true;
    //         } else if (pid == 0) {
    //             if (execvp(backCom[0], backCom) < 0) {
    //                 printf("\nCould not execute command..\n");
    //             }
    //             exit(0);
    //         }
    //         for (int j=0; j < backNum; j++) {
    //             free(backCom[j]);
    //         }
    //         backNum = 0;
    //     }
    //     else {
    //         backCom[backNum] = (char*) malloc(strlen(command[i]) + 1);
    //         strcpy(backCom[backNum], command[i]);
    //         backNum++;
    //     }

    // }
    // free(backCom);
    // return false;
}