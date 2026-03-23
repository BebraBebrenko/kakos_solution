#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int launch(FILE *input, FILE *output, int id, int num_iter) {
    int got = 0;

    while (fscanf(input, "%d", &got) == 1 && got + 1 < num_iter) {
        printf("%d %d\n", id, got + 1);
        fflush(stdout);
        fprintf(output, "%d ", got + 1);
    }

    fclose(input);
    fclose(output);

    return 0;
}

typedef struct {
    FILE *input;
    FILE *output;
} pipe_t;

int open_2(pipe_t *pipe, int pfd[2]) {
    pipe->input = fdopen(pfd[0], "r");
    pipe->output = fdopen(pfd[1], "w");
    if (pipe->input == NULL) {
        return -1;
    }
    if (pipe->output == NULL) {
        return -1;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        return 1;
    }

    int num_iter = atoi(argv[1]);

    int pfds[2][2];
    pipe_t pipe_files[2];

    for (int i = 0; i < 2; i++) {
        if (pipe(pfds[i]) < 0) {
            return 1;
        }
    }

    for (int i = 0; i < 2; i++) {
        if (open_2(&pipe_files[i], pfds[i]) == -1) {
            return 1;
        }

        setvbuf(pipe_files[i].output, NULL, _IONBF, 0);
    }

    fprintf(pipe_files[0].output, "%d ", 0);

    for (int i = 0; i < 2; i++) {

        pid_t pid = fork();

        if (pid < 0) {
            return 1;
        }

        if (pid == 0) {
            fclose(pipe_files[1 - i].input);
            fclose(pipe_files[i].output);

            launch(pipe_files[i].input, pipe_files[1 - i].output, i + 1, num_iter);
            return 0;
        }
    }

    for (int i = 0; i < 2; i++) {
        fclose(pipe_files[i].input);
        fclose(pipe_files[i].output);
    }

    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    puts("Done");
    return 0;
}
