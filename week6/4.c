#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int main(int argc, char **argv) {

    if (argc != 2) {
        return 1;
    }

    char *expression = argv[1];
    char *dir_path;
    char *runtime_dir = getenv("XDG_RUNTIME_DIR");
    char *tmp_dir = getenv("TMPDIR");

    if (runtime_dir != NULL) {
        dir_path = runtime_dir;
    } else if (tmp_dir != NULL) {
        dir_path = tmp_dir;
    } else {
        dir_path = "/tmp";
    }

    char full_path[PATH_MAX];
    pid_t pid = getpid();

    if (snprintf(full_path, sizeof(full_path), "%s/%d.c", dir_path, pid) >= PATH_MAX) {
        return 1;
    }

    printf("%s", full_path);
    int fd = open(full_path, O_RDWR | O_CREAT | O_CLOEXEC | O_TRUNC, S_IRWXU);

    if (fd == -1) {
        return 1;
    }
    FILE *source = fdopen(fd, "w");

    fprintf(source,
            "#include <stdio.h>\n"
            "#include <stdint.h>\n"
            "\n"
            "int main(){\n"
            "int32_t x = 0;"
            "char* reject = \"reject\";"
            "char* summon = \"summon\";"
            "char* disqualify = \"disqualify\";"
            "    while (scanf(\"%%d\", &x) == 1) {\n"
            "        char *res = %s;\n"
            "        printf(\"%%s\\n\", res);\n"
            "    }\n"
            "}\n",
            expression);
    fclose(source);

    pid = fork();

    if (pid < 0) {
        return 1;
    }

    if (pid == 0) {
        chdir(dir_path);
        execlp("gcc", "gcc", full_path, NULL);
        exit(1);
    }

    int status = 0;

    wait(&status);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        return 1;
    }

    if (snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, "a.out") >= PATH_MAX) {
        return 1;
    }

    execlp(full_path, full_path, NULL);

    return 1;
}
