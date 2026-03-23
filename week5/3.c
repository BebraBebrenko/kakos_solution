#include <sys/types.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#define PROCESS_FAIL 65

uint8_t process(int64_t num) {
    uint8_t result = 0;
    uint64_t unsigned_num = num;

    while (unsigned_num) {
        if (unsigned_num & 1) {
            result++;
        }
        unsigned_num = unsigned_num >> 1;
    }

    return result;
}

int main(int argc, char **argv) {

    if (argc < 2) {
        return 0; // НУ И БРЕД, runtime error из-за этого
    }

    int n_proc = argc - 1;

    uint8_t *result_table = malloc(n_proc * sizeof(*result_table));
    pid_t *pid_table = malloc(n_proc * sizeof(*pid_table));

    for (int i = 0; i < n_proc; i++) {

        pid_t pid = fork();

        if (pid == 0) {

            char *endptr = NULL;
            errno = 0;
            int64_t num = strtoll(argv[i + 1], &endptr, 10);

            if (errno != 0 || endptr == argv[i + 1]) {
                return PROCESS_FAIL;
            }

            free(result_table);
            free(pid_table);
            return process(num);
        } else {
            pid_table[i] = pid;
        }
    }

    int status = 0;
    int children_error = 0;
    for (int i = 0; i < n_proc; i++) {

        pid_t exited = wait(&status);

        if (exited == -1 || !WIFEXITED(status) || WEXITSTATUS(status) == PROCESS_FAIL) {
            children_error = 1;
        }

        for (int j = 0; j < n_proc; j++) {
            if (pid_table[j] == exited) {
                result_table[j] = WEXITSTATUS(status);
            }
        }
    }
    if (!children_error) {
        for (int i = 0; i < n_proc; i++) {
            printf("%d\n", result_table[i]);
        }
    }
    free(result_table);
    free(pid_table);

    return children_error;
}
