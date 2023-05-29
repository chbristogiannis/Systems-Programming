#include "../include/history.h"
#include "../include/common.h"
#include <readline/history.h>

#include <stdlib.h>
#include <string.h>

// This function updates the command line history by adding the current line to the history with add_history
// and removes the oldest entry if the history has reached MAX_HISTORY
void historyUpdate(char* line) {
    
    add_history(line);
    
    using_history();
    HISTORY_STATE *myhist = history_get_history_state();
    
    if (myhist->length <= MAX_HISTORY)
        return;
    
    HIST_ENTRY *entry;
    entry = remove_history(0);
    free(entry);
}

// The function first parses the user input to determine whether the user has requested 
// to view previous commands or to execute a specific command from the history.
// if the user wants to view the history, the function retrieves the history and print it.
// If the user wants to execute a specific command from the history, 
// the function retrieves the corresponding command from the history 
// and copies the command to the input string passed as a parameter. 
// The function then returns true to indicate that the user input has been modified.
bool historyHandler(char** line) {

    int index;
    char* line_copy = strdup(*line);
    strtok(line_copy, " ");
    char* word2 = strtok(NULL, " ");
    
    using_history();
    HISTORY_STATE *myhist = history_get_history_state();
    HIST_ENTRY *mylist;
    if (word2 == NULL) {

        printf("\nHistory, length %d:\n", myhist->length);
        for (int i = 0; i < myhist->length; i++) {
            mylist = history_get(i + 1);
            printf("\n%d. %8s", myhist->length - i, mylist->line);
        }
    }
    else if ((index = atoi(word2)) != 0) {
        if (myhist->length == myhist->length - (index)) 
            printf("\n!ERROR!, myhistory can't reference itself\n");
        
        else if (myhist->length >= index) {
            mylist = history_get(myhist->length - index  + 1);
            strcpy(*line, mylist->line);
            free(line_copy);
            return true;
        }
    }
    else {
        printf("\nindex must be a integer or NULL\n");
    }

    free(line_copy);
    return false;
}

