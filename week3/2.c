#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int comp(const void *a, const void *b) {
    double fa = *(const double *) a;
    double fb = *(const double *) b;
    return (fa > fb) - (fa < fb);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        return EXIT_FAILURE;
    }

    struct stat stat_obj;
    if (fstat(fd, &stat_obj) == -1) {
        close(fd);
        return EXIT_FAILURE;
    }

    size_t filesize = stat_obj.st_size;
    size_t num_elem = filesize / sizeof(double);

    if (num_elem == 0) {
        close(fd);
        puts("0");
        return 0;
    }

    double *map = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);
    if (map == MAP_FAILED) {
        return EXIT_FAILURE;
    }

    size_t drop_first = num_elem / 10;
    size_t after_first = num_elem - 2 * drop_first;
    if (after_first == 0) {
        munmap(map, filesize);
        puts("0");
        return 0;
    }

    qsort(map + drop_first, after_first, sizeof(double), comp);

    size_t drop_second = after_first / 10;
    size_t num_left = after_first - 2 * drop_second;
    if (num_left == 0) {
        munmap(map, filesize);
        puts("0");
        return 0;
    }

    double sum = 0;
    for (size_t i = 0; i < num_left; i++) {
        sum += map[drop_first + drop_second + i];
    }

    munmap(map, filesize);
    printf("%.10g\n", sum / num_left);
    return 0;
}
