
#include "bits/types/siginfo_t.h"
#include "fcntl.h"
#include "signal.h"
#include "sys/wait.h"
#include "unistd.h"
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

volatile sig_atomic_t ready = 0;

void handle_ack(int s) { ready = 1; }

void send_data(char *filename, pid_t other) {
    int fd = open(filename, O_RDONLY, NULL);
    char aboba;
    while (read(fd, &aboba, 1) == 1) {
        for (int i = 0; i < CHAR_BIT; i++) {
            char bit = aboba & 1;
            aboba = aboba >> 1;
            if (bit == 0) {
                kill(other, SIGUSR1);
            } else {
                kill(other, SIGUSR2);
            }
            while (!ready) {
                pause();
            }
            ready = 0;
        }
    }
    kill(other, SIGIO);
}

volatile sig_atomic_t bit_recieved = -1;
volatile _Atomic pid_t sender;

void recieve_bit(int s, siginfo_t *info, void *context) {
    sender = info->si_pid;
    if (s == SIGUSR1) {
        bit_recieved = 0;
    } else if (s == SIGUSR2) {
        bit_recieved = 1;
    }
}
void handle_eof(int s) { exit(0); }

void recieve_data() {
    while (1) {
        char byte = 0;
        for (int i = 0; i < CHAR_BIT; i++) {
            while (bit_recieved == -1) {
                pause();
            };
            byte = byte | ((bit_recieved & 1) << i);
            bit_recieved = -1;
            kill(sender, SIGALRM);
        }
        write(STDOUT_FILENO, &byte, 1);
        fflush(stdout);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        return 1;
    }

    // reciever
    struct sigaction sigusr_config = {.sa_flags = SA_RESTART | SA_SIGINFO, .sa_sigaction = &recieve_bit};
    struct sigaction sigio_config = {.sa_flags = SA_RESTART, .sa_handler = &handle_eof};

    sigaction(SIGUSR1, &sigusr_config, NULL);
    sigaction(SIGUSR2, &sigusr_config, NULL);
    sigaction(SIGIO, &sigio_config, NULL);

    // sender
    struct sigaction config = {.sa_flags = SA_RESTART, .sa_handler = &handle_ack};

    sigaction(SIGALRM, &config, NULL);

    pid_t pid1 = fork();
    if (pid1 == 0) {

        recieve_data();
        return 0;
    }
    pid_t pid2 = fork();
    if (pid2 == 0) {
        send_data(argv[1], pid1);
        return 0;
    }

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
