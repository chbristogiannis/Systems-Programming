#pragma once
#include "../common.h"


// Data to sent to the thread
typedef struct {
    char name[MAX_NAME_LENGTH];
    char vote[MAX_VOTE_LENGTH];
    char serverName[MAX_SERVER_LENGTH];
    int portNum;
} ClientThreadData;


// Thread fuction
void *voteThread(void *arg);
