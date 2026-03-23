#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define NPROC 3
#define LINE_SIZE 8

int main() {
    for (int i = 1; i <= NPROC; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            char buff[LINE_SIZE + 1] = {0};
            ssize_t n_read = read(0, buff, LINE_SIZE);
            if (n_read == -1) {
                return 1;
            }
            char *endptr;
            errno = 0;
            int32_t num = strtol(buff, &endptr, 10);
            if (errno != 0 || endptr == buff) {
                return 1;
            }
            printf("%d %d\n", i, num * num);
            return 0;
        }
    }

    int status;
    for (int i = 0; i < NPROC; i++) {
        wait(&status);
        if (WEXITSTATUS(status) != 0) {
            return 1;
        }
    }
    return 0;
}
