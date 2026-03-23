#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// 1 on ok
// 0 on faliure
int execute(char *name) {
    pid_t pid = fork();
    if (pid < 0) {
        return 1;
    }
    if (pid == 0) {
        execlp(name, name, NULL);
        return 0;
    }
    int status = 0;
    int exited = waitpid(pid, &status, 0);
    return exited >= 0 && WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        return 1;
    }

    int ok = execute(argv[1]);

    if (!ok) {
        ok = execute(argv[2]);
    }
    if (ok) {
        ok = execute(argv[3]);
    }
    return !ok;
}
