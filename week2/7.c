#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
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
    struct stat current_dir;
    if (stat(".", &current_dir) == -1) {
        return -1;
    }

    DIR *parent_dir = opendir("..");
    if (!parent_dir) {
        return -1;
    }

    struct dirent *entry;

    while ((entry = readdir(parent_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (entry->d_ino != current_dir.st_ino) {
            continue;
        }

        strncpy(buff, entry->d_name, NAME_MAX);
        buff[NAME_MAX] = '\0';

        closedir(parent_dir);
        return 0;
    }
    closedir(parent_dir);

    return -1;
}

ssize_t getcwd2(int fd, char *buf, size_t size) {
    if (size == 0) {
        buf[0] = '\0';
        return 0;
    }
    size--;
    int work_dir = open(".", O_RDONLY);
    if (work_dir == -1) {
        return 1;
    }
    if (fchdir(fd) != 0) {
        return 2;
    }
    if (fchdir(fd) != 0) {
        return 3;
    };
    char buff[NAME_MAX + 1];
    vector(char *) dirs = {0};

    while (get_name(buff) == 0) {
        push(dirs, strdup(buff));
        chdir("..");
    }

    if (dirs.size == 0) {
        buf[0] = '/';
        buf[1] = '\0';
    } else {
        int offset = 0;

        for (int i = dirs.size - 1; i >= 0; i--) {
            if (offset >= size) {
                break;
            }

            int written = snprintf(buf + offset, size - offset, "/%s", dirs.data[i]);

            if (written < 0) {
                break;
            }
            if (written >= size - offset) {
                offset = size - 1;
                break;
            }
            offset += written;
        }
    }

    vec_free(dirs);
    fchdir(work_dir);
    close(work_dir);
    return 0;
};

int main(int argc, char **argv) {
    char buff[PATH_MAX + 1];
    int fd = open("/usr/local/bin", O_RDONLY);
    getcwd2(fd, buff, PATH_MAX);
    printf("%s", buff);
    return 0;
}

//    char buff[NAME_MAX + 1];
//     vector(char *) dirs = {0};

//     while (get_name(buff) == 0) {
//         push(dirs, strdup(buff));
//         chdir("..");
//     }

//     if (dirs.size == 0) {
//         printf("/\n");
//     } else {
//         for (int i = dirs.size - 1; i >= 0; i--) {
//             printf("/%s", dirs.data[i]);
//         }
//         puts("");
//     }

//     vec_free(dirs);
