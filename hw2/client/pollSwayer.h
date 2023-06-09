#pragma once
#include "../common.h"

typedef struct {
    char name[MAX_NAME_LENGTH];
    char vote[MAX_VOTE_LENGTH];
    char serverName[MAX_SERVER_LENGTH];
    int portNum;
} ClientThreadData;




void *voteThread(void *arg);


