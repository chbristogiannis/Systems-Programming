#pragma once

#include "common.h"
#include <stdbool.h>

// This function is used to handle input and output redirection for a command. It takes as input the command as an array of strings 
// (each element of the array representing a word in the command), 
// the number of words in the command, and a boolean variable indicating whether the command should be executed in the background.
// The function returns a boolean value depending on error.
bool execRedirection(char** , int, bool);

