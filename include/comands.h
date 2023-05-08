#pragma once

#include "common.h"
#include <stdbool.h>

// These are fuctions that sets the flags about how a command need to be executed
// Info1 takes a char** and checks word for word
void comInfo1(char** command, int numOfCom, bool* is_redirection, bool* is_piped, bool* is_background, bool* has_wildcard, bool* create_aliases, bool* mult_com);

// These are fuctions that sets the flags about how a command need to be executed
// Info2 takes a command and searches for characters
void comInfo2(char* command, bool* is_redirection, bool* is_piped, bool* is_background, bool* has_wildcard, bool* create_aliases, bool* mult_com);


// This is a function called comBrk1 that takes parameters command, remove, and comNum
// and returns a the command broken in peaces every time it finds the char remove
// and returns the size of the new vector
char*** comBrk1(char**, char*, int*);

// This is a function called comBrk2 that takes parameters command, remove, and comNum
// and returns a the command broken in peaces every time it finds the char remove
// and returns the size of the new vector
char** comBrk2(char*, char*, int*);


// This is a function called comSimpleExec that takes two parameters: command, which is an array of strings representing the command to be executed, 
// and is_back, which is a boolean flag indicating whether the command should be run in the background.
void comSimpleExec(char**, bool);

// he comAliasesReplace function takes in a command string, an array of aliases, an array of alias values, and the number of aliases. 
// It replaces any occurrences of an alias in the command string with its corresponding value.
void comAliasesReplace(char*, char**, char**, int);

// This function multiComHandlere takes a command array and the number of commands in it, numOfCom
// breaks each command and execute it
void multiComHandlere(char** commands, int numCom);

int countArgs(char **command) ;