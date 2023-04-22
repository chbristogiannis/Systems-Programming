#include <stdio.h>

#include <string.h>
#include <stdbool.h>

#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>


int main (void) {
    
    printf("in-mysh-now:>");

    // Read Line
    char* line = NULL;
    size_t len = 0;
    if (getline(&line, &len, stdin) == -1) {
            perror("getline");
            exit(EXIT_FAILURE);
    }
    
    bool is_piped = false;
    if (strstr(line, "|") != NULL)
        is_piped = true;

    char **words = NULL;
    char *word = strtok(line, " ");
    int num_words = 0;
    while (word != NULL && strcmp(word, "\n") != 0) {
        words = (char **)realloc(words, sizeof(char *) * (num_words + 1));  // resize array to fit new word
        words[num_words] = (char *)malloc(strlen(word) + 1);    // allocate memory for new word and copy it
        strcpy(words[num_words], word);

        num_words++;
        word = strtok(NULL, " ");
    }

    if (! is_piped) {
        char* input_file = NULL;
        char *output_file = NULL;
        char* append_file = NULL;
        char* command = words[0];
        for (int i=1; i < num_words; i++) {
            // printf("%s\n", words[i]);
            if (strcmp(words[i], "<") == 0) {
                if (i+1 == num_words) {
                    printf("Not input given\n");
                    return -1;
                }
                input_file = words[i+1];
            }
            else if (strcmp(words[i], ">") == 0) {
                if (i+1 == num_words) {
                    printf("Not output given\n");
                    return -1;
                }
                output_file = words[i+1];
            }
            else if (strcmp(words[i], ">>") == 0) {
                if (i+1 == num_words) {
                    printf("Not output given\n");
                    return -1;
                }
                append_file = words[i+1];
            }
            else {
                if (!(strcmp(words[i-1], ">") == 0 || strcmp(words[i-1], "<") == 0 || strcmp(words[i-1], ">>") == 0)) {
                    input_file = words[i];
                }
            }
        }

        printf("%s %s %s", command, input_file, output_file);

        if (input_file) {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            
            if (dup2(fd_in, STDIN_FILENO) == -1) {
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(fd_in);
        }
        
        if (output_file) {
            int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            
            if (dup2(fd_out, STDOUT_FILENO) == -1) {
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(fd_out);
        }
        
        if (append_file) {
            int fd_append = open(append_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd_append == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            
            if (dup2(fd_append, STDOUT_FILENO) == -1) {
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(fd_append);
        }

        execvp(command, NULL);

    }
    
    
    // free memory
    for (int i = 0; i < num_words; i++) {
        free(words[i]);
    }
    free(words);
    // free(line);
    
    return 0;