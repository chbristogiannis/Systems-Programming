#include "../include/history.h"

#include <readline/history.h>

#include <stdlib.h>
#include <string.h>
#define MAX_HISTORY 20

void update_history(char* line) {
    add_history(line);
    using_history();
    HISTORY_STATE *myhist = history_get_history_state();
    HIST_ENTRY *entry;

    if (myhist->length <= MAX_HISTORY)
        return;
    entry = remove_history(0);
    free(entry);
}

bool historyHandler(char** line) {

    int index;
    char* line_copy = strdup(*line);
    char* word1 = strtok(line_copy, " ");
    char* word2 = strtok(NULL, " ");
    
    using_history();
    HISTORY_STATE *myhist = history_get_history_state();
    HIST_ENTRY *mylist;
    if (word2 == NULL) {

        printf("\nHistory, length %d:\n", myhist->length);
        for (int i = 0; i < myhist->length; i++) {
            mylist = history_get(i + 1);
            printf("%d. %8s", myhist->length - i, mylist->line);
        }
    }
    else if ((index = atoi(word2)) != 0) {
        if (myhist->length == myhist->length - (index)) 
            printf("\n!ERROR!, myhistory can't reference itself\n");
        
        else if (myhist->length >= index) {
            mylist = history_get(index);
            strcpy(*line, mylist->line);
            free(line_copy);
            return true;
        }
    }
    else 
        printf("\nindex must be a integer or NULL\n");

    free(line_copy);
    return false;
}