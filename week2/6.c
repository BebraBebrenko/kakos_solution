#include "fcntl.h"
#include "sys/stat.h"
#include "time.h"
#include "unistd.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define vector(T)                                                                                                      \
    struct {                                                                                                           \
        T *data;                                                                                                       \
        size_t size;                                                                                                   \
        size_t capacity;                                                                                               \
    }

#define push(vec, value)                                                                                               \
    {                                                                                                                  \
        if ((vec).size >= (vec).capacity) {                                                                            \
            (vec).capacity = (vec).capacity ? (vec).capacity * 2 : 8;                                                  \
            (vec).data = realloc((vec).data, (vec).capacity * sizeof(*(vec).data));                                    \
            if ((vec).data == NULL) {                                                                                  \
                exit(EXIT_FAILURE);                                                                                    \
            }                                                                                                          \
        }                                                                                                              \
        (vec).data[(vec).size++] = (value);                                                                            \
    }

#define vec_free(vec)                                                                                                  \
    for (size_t i = 0; i < vec.size; i++) {                                                                            \
        free(vec.data[i]);                                                                                             \
    }                                                                                                                  \
    (free((vec).data), (vec).data = NULL, (vec).size = (vec).capacity = 0)

int get_name(char *buff) {
    struct stat target_stat;
    if (stat(".", &target_stat) == -1) {
        return -1;
    }

    DIR *dir = opendir("..");
    if (!dir) {
        return -1;
    }

    struct dirent *entry;
    int found = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_ino == target_stat.st_ino) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            strncpy(buff, entry->d_name, NAME_MAX - 1);
            buff[NAME_MAX - 1] = '\0';
            found = 1;
            break;
        }
    }

    closedir(dir);
    return found ? 0 : -1;
}

ssize_t getcwd2(int fd, char *buf, size_t size) {
    if (size == 0) {
        buf[0] = '\0';
        return 0;
    }
    int work_dir = open(".", O_DIRECTORY | O_RDONLY);
    if (work_dir == -1) {
        return 1;
    }
    if (fchdir(fd) != 0) {
        return 2;
    }
    vector(char *) dirs;
    if (fchdir(fd) != 0) {
        return 3;
    };

    char initial_dir[NAME_MAX];
    get_name(initial_dir);
    push(dirs, strdup(initial_dir));

    // for (int i = 0; i < dirs.size; i++) {
    //     printf("%s\n", dirs.data[i]);
    // }
    fchdir(work_dir);
    close(work_dir);
    return 0;
};

int main(int argc, char **argv) {

    char buff[NAME_MAX];
    vector(char *) dirs;

    while (dirs.size < 2 || strcmp(buff, dirs.data[dirs.size - 2])) {
        if (get_name(buff) < 0) {
            break;
        }
        push(dirs, strdup(buff));

        chdir("..");

        puts(buff);
    }
    vec_free(dirs);
    return 0;
}
