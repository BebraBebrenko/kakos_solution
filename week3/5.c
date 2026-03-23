#include "fcntl.h"
#include "sys/mman.h"
#include "sys/stat.h"
#include <stdlib.h>
int main(int argc, char **argv) {
    if (argc < 2) {
        return EXIT_FAILURE;
    }
    int fd = open(argv[1], O_RDONLY);

    struct stat stat_obj;
    if (fstat(fd, &stat_obj) == -1) {
        return EXIT_FAILURE;
    }

    off_t file_size = stat_obj.st_size;
    char *map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
}
