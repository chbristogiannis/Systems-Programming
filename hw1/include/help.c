#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void handle_sigint(int sig) {
    printf("Child process received SIGINT signal. Exiting...\n");
    exit(0);
}

void handle_sigtstp(int sig) {
    printf("Child process received SIGTSTP signal. Suspending...\n");
    kill(getpid(), SIGSTOP);
}

int main() {
    pid_t pid;
    int status;

    // Fork child process
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        // Child process

        // Set up signal handlers in child process
        signal(SIGINT, handle_sigint);
        signal(SIGTSTP, handle_sigtstp);

        // Loop indefinitely
        int i = 0;
        while (i < 15) {
            printf("Child process executing...\n");
            sleep(1);
            i++;
        }

        // Exit child process
        exit(0);
    } else {
        // Parent process

        // Set up signal handlers in parent process
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);

        int i = 0;
        while (i < 15) {
            printf("Parent process executing...\n");
            sleep(1);
            i++;
        }

        // Wait for child process to finish
        wait(&status);

        // Exit parent process
        printf("Parent process exiting...\n");
        exit(0);
    }

    // This code should never be reached
    return 0;
}
