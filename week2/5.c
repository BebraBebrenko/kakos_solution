#include "fcntl.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

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

int main(int argc, char **argv) {
    if (argc < 3) {
        exit(EXIT_FAILURE);
    }

    char *root_dir = strdup(argv[1]);

    long long max_size = atoll(argv[2]);

    int prefix_length = (int) strlen(root_dir);

    vector(char *) directories = {0};
    push(directories, root_dir);

    size_t current_i_start = 0;

    for (int depth = 0; depth < 4; depth++) {
        size_t current_i_end = directories.size;

        for (size_t i = current_i_start; i < current_i_end; i++) {
            DIR *dir_ptr = opendir(directories.data[i]);
            if (dir_ptr == NULL) {
                continue;
            }

            struct dirent *entry;
            while ((entry = readdir(dir_ptr)) != NULL) {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    continue;
                }
                char abs_path[PATH_MAX];
                if (snprintf(abs_path, PATH_MAX, "%s/%s", directories.data[i], entry->d_name) >= PATH_MAX) {
                    continue;
                }
                int fd = open(abs_path, O_RDONLY | O_NOFOLLOW);
                if (fd == -1) {
                    continue;
                }
                struct stat stat_object;
                if (fstat(fd, &stat_object) != 0) {
                    continue;
                }
                if (S_ISLNK(stat_object.st_mode)) {
                    continue;
                }
                if (S_ISREG(stat_object.st_mode) && stat_object.st_size <= max_size && access(abs_path, R_OK) != -1) {
                    char *start = abs_path + prefix_length;
                    if (*start == '/') {
                        start++;
                    }
                    printf("%s\n", start);
                } else if (S_ISDIR(stat_object.st_mode)) {
                    push(directories, strdup(abs_path));
                }
            }
            closedir(dir_ptr);
        }
        current_i_start = current_i_end;
    }
    vec_free(directories);

    exit(EXIT_SUCCESS);
}
