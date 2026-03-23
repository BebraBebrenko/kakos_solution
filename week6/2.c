#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

typedef struct state {
    int pending;
    int total_success;
} state;

void wait_for_all(state *s) {
    while (s->pending) {
        int status = 0;
        pid_t exited = wait(&status);
        if (exited != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            s->total_success++;
        }
        s->pending--;
    }
}
int execute(state *s, char *name) {
    pid_t pid = fork();

    if (pid < 0) {
        return -1;
    }

    if (pid == 0) {
        execlp(name, name, NULL);
        return -1;
    }
    s->pending++;
    return 0;
}
int main(int argc, char **argv) {
    if (argc < 2) {
        puts("0");
        return 0;
    }
    state global_state = {0};
    for (int i = 1; i < argc; i++) {
        if (*argv[i] == 'p') {
            if (execute(&global_state, argv[i] + 1) == -1) {
                return 1;
            }
            continue;
        }
        if (*argv[i] == 's') {

            wait_for_all(&global_state);
            if (execute(&global_state, argv[i] + 1) == -1) {
                return 1;
            }
            continue;
        }
        return 1;
    }
    wait_for_all(&global_state);
    printf("%d\n", global_state.total_success);
    return 0;
}
