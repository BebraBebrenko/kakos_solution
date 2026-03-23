#include "fcntl.h"
#include "sys/mman.h"
#include "unistd.h"
#include <math.h>
#include <stdlib.h>
int main(int argc, char **argv) {
    if (argc < 4) {
        return EXIT_FAILURE;
    }
    int width = atoi(argv[2]);
    int height = atoi(argv[3]);
    int file_size = width * height * sizeof(double);
    int fd = open(argv[1], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        return 1;
    }
    if (ftruncate(fd, file_size) == -1) {
        return 1;
    }
    double *map = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (map == MAP_FAILED) {
        return 1;
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            map[i * height + j] = 2 * sin((double) i) + 4 * cos(((double) j) / 2.0);
        }
    }
    munmap(map, file_size);
    return EXIT_SUCCESS;
}
