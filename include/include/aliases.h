#pragma once

#include "common.h"
#include <stdbool.h>

// This function handles the creation and destruction of command aliases.
// It takes as input an array of command arguments, the number of command arguments, 
// pointers to the current aliases and their values, and a pointer to the number of aliases.
void aliasesHandler(char** , int , char*** , char*** , int*);