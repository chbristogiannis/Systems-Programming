#include "../include/background.h"
#include "../include/comands.h"
#include "../include/pipes.h"
#include "../include/redirection.h"
#include "../include/wildcharacters.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>




// The function starts by breaking up the command list into separate commands using the comBrk1() function. 
// Then it goes through each command in the list and analyzes it for certain features, 
// such as whether it contains a pipe (is_piped), a redirection (is_redirection), or a wildcard (has_wildcard).
// and then execute them because we sent flag true to the other fuctions we do not wait for each one to end before we start.
void backgroundComAnalysis(char** command, int numOfCom) {
    
    int backNum = 0;
    char*** backCom = comBrk1(command, "&", &backNum);

    bool is_redirection, is_piped, is_background, has_wildcard, create_aliase, is_history;
    
    for (int i =0; backCom[i] != NULL; i++) { 
        int counter = countArgs(backCom[i]);
        comInfo1(backCom[i], counter, &is_redirection, &is_piped, &is_background, &has_wildcard, &create_aliase, &is_history);
        if (is_piped) {
            pipeComAnalysis(backCom[i], counter, true);
        }
        else if (has_wildcard) {
           wildcardsComAnalysis(backCom[i], counter, true);
        }
        else if (is_redirection) {
            if (execRedirection(backCom[i], counter, true));
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
}