#include "fcntl.h"
#include "sys/wait.h"
#include "unistd.h"
#include <stdint.h>
#include <stdio.h>
int main() {
    int pfd[2];

    if (pipe(pfd) < 0) {
        return 1;
    };

    pid_t pid = fork();
    if (pid < 0) {
        return 1;
    }

    if (pid == 0) {
        close(pfd[1]);
        pid = fork();

        if (pid < 0) {
            return 1;
        }

        if (pid == 0) {
            ssize_t n_read = 0;
            int64_t total = 0;
            int32_t current = 0;

            while ((n_read = read(pfd[0], &current, sizeof(current)) > 0)) {
                total += current;
            }

            close(pfd[0]);
            if (n_read < 0) {
                return 1;
            }
            printf("%ld\n", total);
            return 0;
        }

        close(pfd[0]);

        wait(NULL);
        return 0;
    }
    close(pfd[0]);

    ssize_t n_read = 0;
    int32_t buff;

    while ((n_read = scanf("%d", &buff)) == 1) {
        if (write(pfd[1], &buff, sizeof(buff)) < 0) {
            return 1;
        };
    }
    close(pfd[1]);
    wait(NULL);
    return 0;
}
