#include <stddef.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void kill_all_panic(pid_t *arr, size_t size) {
    for (int i = 0; i < size; i++) {
        kill(arr[i], SIGKILL);
    }

    exit(1);
}

int main(int argc, char **argv) {
    pid_t *pids = malloc(argc * sizeof(pid_t));
    size_t size_pids = 0;
    int input_fd = STDIN_FILENO;
    int pfd[2];

    int panic_chan[2];

    if (argc < 2) {
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        if (i != argc - 1) {
            if (pipe(pfd) < 0) {
                kill_all_panic(pids, size_pids);
            }
        }
        if (pipe(panic_chan) < 0) {
            kill_all_panic(pids, size_pids);
        };
        pid_t pid = fork();
        if (pid < 0) {
            kill_all_panic(pids, size_pids);
        }
        if (pid == 0) {
            close(panic_chan[0]);

            if (i != argc - 1) {
                close(pfd[0]);
                if (dup2(pfd[1], STDOUT_FILENO) < 0) {
                    kill_all_panic(pids, size_pids);
                }
                close(pfd[1]);
            }

            if (dup2(input_fd, STDIN_FILENO) < 0) {
                kill_all_panic(pids, size_pids);
            }
            if (input_fd != STDIN_FILENO) {
                close(input_fd);
            }

            fcntl(panic_chan[1], F_SETFD, FD_CLOEXEC);

            execlp(argv[i], argv[i], NULL);
            char smth = 'Z';
            write(panic_chan[1], &smth, 1);
            return 1;
        }

        char smth = 0;
        close(panic_chan[1]);
        if (read(panic_chan[0], &smth, 1) > 0) {
            kill_all_panic(pids, size_pids);
        }
        close(panic_chan[0]);

        if (i != argc - 1) {
            if (close(pfd[1]) == -1) {
                kill_all_panic(pids, size_pids);
            }
        }
        pids[size_pids++] = pid;

        if (input_fd != STDIN_FILENO) {
            close(input_fd);
        }
        if (i != argc - 1) {
            input_fd = pfd[0];
        }
    }
    for (int i = 1; i < argc; i++) {
        wait(NULL);
    }
    free(pids);
    return 0;
}
