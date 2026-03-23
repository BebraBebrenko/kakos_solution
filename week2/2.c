#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define EXE_EXTENSION ".exe"

int concat_path(char *out, size_t out_size, const char *dir, const char *name) {
    int path_len = snprintf(out, out_size, "%s/%s", dir, name);
    if (path_len <= 0 || (size_t) path_len >= out_size) {
        out[0] = '\0';
        return 0;
    }

    return 1;
}

int suffix(const char *str, const char *suffix_str) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix_str);

    if (str_len < suffix_len) {
        return 0;
    }
    return strcmp(str + (str_len - suffix_len), suffix_str) == 0;
}

int is_exe(const char *dir_path, struct dirent *entry) {
    struct stat stat_obj;
    char abs_path[PATH_MAX];

    if (!concat_path(abs_path, sizeof(abs_path), dir_path, entry->d_name)) {
        return 0;
    }

    if (stat(abs_path, &stat_obj) != 0) {
        return 0;
    }

    if (!S_ISREG(stat_obj.st_mode)) {
        return 0;
    }

    if (access(abs_path, X_OK) == -1) {
        return 0;
    }

    if (!suffix(entry->d_name, EXE_EXTENSION)) {
        return 0;
    }

    return 1;
}

int main(int argc, char **argv) {
    int total_count = 0;

    if (argc < 2) {
        printf("%d\n", total_count);
        return 0;
    }

    DIR *dir_ptr = opendir(argv[1]);
    if (dir_ptr == NULL) {
        printf("%d\n", total_count);
        return 0;
    }

    struct dirent *entry;
    errno = 0;
    while ((entry = readdir(dir_ptr)) != NULL) {
        total_count += is_exe(argv[1], entry);
        errno = 0;
    }
    if (errno != 0) {
        closedir(dir_ptr);
        return 1;
    }
    if (closedir(dir_ptr) == -1) {
        return 1;
    }

    printf("%d\n", total_count);
    return 0;
}
