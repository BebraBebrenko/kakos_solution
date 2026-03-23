#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    int number = 0;
    pid_t pid = 0;
    int my_number = 0;
    while (scanf("%d", &number) == 1) {
        if ((pid = fork()) < 0) {
            return 1;
        }
        if (pid != 0) {
            my_number = number;
            waitpid(pid, NULL, 0);
            printf("%d\n", my_number);

            break;
        }
    }

    return 0;
}
