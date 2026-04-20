#include <bits/types/sigset_t.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

volatile sig_atomic_t flag;
void handler(int s) { flag = 1; }

#define RESTART_COUNT 5 - 1
int main() {

    sigset_t before;
    sigset_t after;
    struct sigaction config = {.sa_handler = handler, .sa_flags = SA_RESTART};
    sigaction(SIGUSR1, &config, NULL);

    sigemptyset(&before);
    sigaddset(&before, SIGUSR1);
    sigprocmask(SIG_BLOCK, &before, NULL);
    sigemptyset(&after);

    pid_t pid = getpid();

    printf("%d\n", pid);
    fflush(stdout);
    int restart_count = RESTART_COUNT;
    int sig_number = 0;
    while (restart_count != 0) {
        while (!flag) {
            sigsuspend(&after);
        }
        flag = 0;
        printf("%d\n", sig_number);
        fflush(stdout);
        sig_number++;
        restart_count--;
    }
    return 0;
}
