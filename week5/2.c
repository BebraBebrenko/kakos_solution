#include "stdlib.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main() {
    int n_proc = 0;
    if (scanf("%d", &n_proc) != 1) {
        return 1;
    }

    int num_to_print = 1;
    pid_t pid;

    for (num_to_print = 1; num_to_print <= n_proc; num_to_print++) {
        pid = fork();

        if (pid != 0) {
            if (num_to_print == n_proc) {
                printf("%d", num_to_print);
            } else {
                printf("%d ", num_to_print);
            }
            fflush(stdout);

            int status;
            wait(&status);
            if (WEXITSTATUS(status) != 0) {
                return 1;
            }
            break;
        }
    }

    if (num_to_print != 1 || pid == 0) {
        return 0;
    }

    puts("");

    return 0;
}
