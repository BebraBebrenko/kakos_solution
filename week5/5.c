#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <sched.h>

#define NPROC 2

int main(int argc, char **argv) {
    if (argc != 2) {
        return 1;
    }
    int n_iter = atoi(argv[1]);

    int *map = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);

    if (map == MAP_FAILED) {
        return 1;
    }

    for (int id = 1; id <= NPROC; id++) {
        pid_t pid = fork();

        if (pid == -1) {
            return 1;
        }

        if (pid != 0) {
            continue;
        }
        int current_num = id;

        while (current_num <= n_iter) {
            if (*map != current_num - 1) {
                sched_yield();
                continue;
            }

            printf("%d %d\n", id, current_num);
            fflush(stdout);

            *map = current_num;

            current_num += NPROC;
        }
        return 0;
    }

    for (int i = 1; i <= NPROC; i++) {
        wait(NULL);
    }

    if (munmap(map, sizeof(int)) == -1) {
        return 1;
    }

    return 0;
}
