
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#define N_PROC 2

int main(int argc, char **argv) {
    if (argc != 3) {
        return 1;
    }

    char *commands[N_PROC] = {argv[1], argv[2]};
    int pfd[N_PROC];

#ifdef _GNU_SOURCE
    pipe2(pfd, O_CLOEXEC);
#else
    pipe(pfd);
#endif

    for (int i = 0; i < N_PROC; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            return 1;
        }

        if (pid == 0) {
            dup2(pfd[1 - i], 1 - i);

#ifndef _GNU_SOURCE
            close(pfd[1 - i]);
#endif
            execlp(commands[i], commands[i], NULL);
            return 1;
        }
    }

    for (int i = 0; i < N_PROC; i++) {
        close(pfd[i]);
    }
    for (int i = 0; i < N_PROC; i++) {
        wait(NULL);
    }
    return 0;
}
