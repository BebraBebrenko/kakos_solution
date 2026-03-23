#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd == -1) {
            puts("-1");
            continue;
        }
        struct stat stat_obj;
        if (fstat(fd, &stat_obj) == -1) {
            close(fd);
            continue;
        }
        size_t filesize = stat_obj.st_size;
        if (filesize == 0) {
            puts("0");
            continue;
        }
        char *map = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
        if (map == MAP_FAILED) {
            close(fd);
            continue;
        }
        close(fd);
        size_t result = 0;
        for (size_t i = 0; i < filesize; i++) {
            if (map[i] == '\n') {
                result++;
            }
        }
        if (map[filesize - 1] != '\n') {
            result++;
        }
        if (munmap(map, filesize) == -1) {
            continue;
        }
        printf("%zu\n", result);
    }
    return EXIT_SUCCESS;
}
