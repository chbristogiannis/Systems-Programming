#pragma once

#include "common.h"

void comInfo1(char** command, int numOfCom, bool* is_redirection, bool* is_piped, bool* is_background, bool* has_wildcard, bool* create_aliases, bool* is_history);

void comInfo2(char* command, bool* is_redirection, bool* is_piped, bool* is_background, bool* has_wildcard, bool* create_aliases, bool* is_history);

char*** comBrk1(char**, char*, int*);

char** comBrk2(char*, char*, int*);

void comSimpleExec(char**, bool);

void comAliasesReplace(char*, char**, char**, int);