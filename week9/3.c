#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

volatile sig_atomic_t ready = 0;

void sig_handler(int sig) { ready = 1; }

void child(int id, int pfd[2], int max_val) {
    FILE *read_end = fdopen(pfd[0], "r");
    FILE *write_end = fdopen(pfd[1], "w");
    pid_t other;

    if (id == 1) {
        fprintf(write_end, "%d\n", getpid());
        fflush(write_end);

        while (!ready) {
            pause();
        }
        ready = 0;

        fscanf(read_end, "%d", &other);
    } else {
        fscanf(read_end, "%d", &other);

        fprintf(write_end, "%d\n", getpid());

        fprintf(write_end, "1\n");
        fflush(write_end);

        kill(other, SIGUSR1);
        while (!ready) {
            pause();
        }
        ready = 0;
    }

    while (1) {
        int num;
        fscanf(read_end, "%d", &num);

        if (num >= max_val) {
            fprintf(write_end, "%d\n", num);
            fflush(write_end);
            kill(other, SIGUSR1);
            break;
        }

        printf("%d %d\n", id, num);
        fflush(stdout);

        fprintf(write_end, "%d\n", num + 1);
        fflush(write_end);

        kill(other, SIGUSR1);

        while (!ready) {
            pause();
        }
        ready = 0;
    }

    fclose(read_end);
    fclose(write_end);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 1;
    }

    int max = atoi(argv[1]);

    struct sigaction config = {.sa_flags = SA_RESTART, .sa_handler = sig_handler};
    sigaction(SIGUSR1, &config, NULL);

    int pfd[2];
    if (pipe(pfd) == -1) {
        return 1;
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        return 1;
    }
    if (pid1 == 0) {
        child(1, pfd, max);
    }

    pid_t pid2 = fork();
    if (pid2 == -1) {
        return 1;
    }
    if (pid2 == 0) {
        child(2, pfd, max);
    }

    close(pfd[0]);
    close(pfd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    printf("Done\n");
    return 0;
}
