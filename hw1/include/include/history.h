#pragma once

#include <stdbool.h>

// This function updates the command line history by adding the current line to the history and 
// removing the oldest entry if the history has reached its maximum size.
void historyUpdate(char*) ;

// This code implements a history handler function that allows users to view and use previous commands entered in the shell. 
// The function takes a pointer to a string (char**) that represents the user input, 
// and returns a boolean indicating whether the user input has been modified (true) or not (false).
bool historyHandler(char** );

