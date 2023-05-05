// void handle_sigint(int sig) {
//     // Exit child process on SIGINT signal
//     printf("Child process received SIGINT signal. Exiting...\n");
//     exit(0);
// }

// void handle_sigtstp(int sig) {
//     // Suspend child process on SIGTSTP signal
//     printf("Child process received SIGTSTP signal %d. Suspending...\n", child_pid);
//     // exit(0);
//     kill(child_pid, SIGSTOP);
// }