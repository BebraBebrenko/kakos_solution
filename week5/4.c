#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    if (argc != 6) {
        return 1;
    }

    // у нас это всегда валидно
    off_t n_proc = (off_t) atoi(argv[1]);
    char *output_file = argv[2];
    int a_0 = atoi(argv[3]);
    int d = atoi(argv[4]);
    off_t nums_per_process = (off_t) atoll(argv[5]);

    int fd = open(output_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (fd == -1 || ftruncate(fd, n_proc * nums_per_process * sizeof(int)) == -1) {
        return 1;
    };

    close(fd);

    for (int offset = 0; offset < n_proc; offset++) {
        pid_t pid = fork();
        if (pid == -1) {
            return 1;
        }
        if (pid == 0) {
            int fd_copy = open(output_file, O_RDWR);

            for (off_t i = 0; i < nums_per_process; i++) {

                off_t write_pos = offset + i * n_proc;
                int output = a_0 + d * write_pos;

                if (lseek(fd_copy, write_pos * sizeof(output), SEEK_SET) == -1) {
                    close(fd_copy);
                    return 1;
                };
                if (write(fd_copy, &output, sizeof(output)) != sizeof(output)) {
                    close(fd_copy);
                    return 1;
                }
            }
            close(fd_copy);
            return 0;
        }
    }

    close(fd);

    int status = 0;
    int children_error = 0;

    for (int i = 0; i < n_proc; i++) {
        pid_t exited = wait(&status);
        if (exited == -1 || !WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            children_error = 1;
        }
    }

    return children_error;
}
