#pragma once

#include "common.h"
#include <stdbool.h>

// This function that performs the execution of a command that contains pipes. The input parameters are:
// the commands, the number of commands and an indicator if its called by background execution
// The function returns a boolean value depending on error.
bool pipeComAnalysis(char** , int , bool); 