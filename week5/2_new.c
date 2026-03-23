#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
int main() {
    int n_proc = 0;
    if (scanf("%d", &n_proc) != 1) {
        return 1;
    }
    pid_t pid = 0;
    int is_main = 0;
    for (int i = 1; i <= n_proc; i++) {
        if (i != n_proc) {
            pid = fork();
        } else {
            pid = 0; // просто имитация жизни
        }
        is_main = 0;
        if (pid != 0) { // мы main
            if (i == 1) {
                is_main = 1;
            }

            if (i != n_proc) {
                printf("%d ", i);
            } else {
                printf("%d", i);
            }
            fflush(stdout);

            break;
        }
    }
    if (!is_main) {
        return 0;
    }
    int status;
    for (int i = 0; i <= n_proc; i++) {
        wait(&status);
        if (WEXITSTATUS(status) != 0) {
            return 1;
        }
    }
    puts("");
}
