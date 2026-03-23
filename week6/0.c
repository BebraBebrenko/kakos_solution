#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char **argv) {

    if (argc != 5) {
        return 1;
    }
    char *executable = argv[1];
    char *stdin_file = argv[2];
    char *stdout_file = argv[3];
    char *stderr_file = argv[4];

    pid_t pid = fork();

    if (pid < 0) {
        return 1;
    }

    if (pid == 0) {
        int flags = S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP;

        int stdin_fd = open(stdin_file, O_RDONLY | O_CLOEXEC);
        int stdout_fd = open(stdout_file, O_WRONLY | O_CLOEXEC | O_CREAT | O_APPEND, flags);
        int stderr_fd = open(stderr_file, O_WRONLY | O_CLOEXEC | O_CREAT | O_TRUNC, flags);

        if (stdin_fd < 0 || stdout_fd < 0 || stderr_fd < 0) {
            return 42;
        }
        int result = 0;

        result = dup2(stdin_fd, STDIN_FILENO);
        if (result < 0) {
            return 42;
        }
        result = dup2(stdout_fd, STDOUT_FILENO);
        if (result < 0) {
            return 42;
        }
        result = dup2(stderr_fd, STDERR_FILENO);
        if (result < 0) {
            return 42;
        }

        execlp(executable, executable, NULL);
        return 42;
    }

    int status = 0;
    pid_t exited = wait(&status);

    if (exited == -1) {
        // unreachable?
        return 1;
    }
    printf("%d\n", status);
    return 0;
}
