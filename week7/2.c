#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#define unwrap(result)                                                                                                 \
    if ((result) < 0) {                                                                                                \
        return 1;                                                                                                      \
    }

// returns pid or -1 on error
pid_t execute(char *cmd, int override_stdin, int override_stdout, int close_fd) {
    pid_t pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid != 0) {
        return pid;
    }

    if (override_stdin >= 0) {
        dup2(override_stdin, STDIN_FILENO);
        close(override_stdin);
    }
    if (override_stdout >= 0) {
        dup2(override_stdout, STDOUT_FILENO);
        close(override_stdout);
    }
    if (close_fd >= 0) {
        close(close_fd);
    }
    execlp(cmd, cmd, NULL);
    return -1;
}

int main(int argc, char **argv) {
    if (argc != 5) {
        return 1;
    }

    char *cmd1 = argv[1];
    char *cmd2 = argv[2];
    char *cmd3 = argv[3];
    char *output_file = argv[4];

    int pfd[2];
    unwrap(pipe(pfd))

        int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    unwrap(output_fd);

    pid_t pid3 = execute(cmd3, pfd[0], output_fd, pfd[1]);
    unwrap(pid3);

    close(pfd[0]);
    close(output_fd);

    pid_t pid1 = execute(cmd1, -1, pfd[1], pfd[0]);
    unwrap(pid1);

    waitpid(pid1, NULL, 0);

    pid_t pid2 = execute(cmd2, -1, pfd[1], pfd[0]);
    unwrap(pid2);

    waitpid(pid2, NULL, 0);

    close(pfd[1]);

    waitpid(pid3, NULL, 0);

    return 0;
}
