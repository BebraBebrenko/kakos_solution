#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

#define KB (1024)
#define KB_MASK (KB - 1)

#define SKIP_IF(condition)                                                                                             \
    if (condition)                                                                                                     \
        return 0;

static int64_t file_size(const char *path) {
    struct stat st;
    SKIP_IF(lstat(path, &st) != 0);
    SKIP_IF(S_ISLNK(st.st_mode));
    SKIP_IF(!S_ISREG(st.st_mode));
    SKIP_IF(st.st_size % KB);
    SKIP_IF(st.st_nlink != 1);
    return st.st_size;
}

int main(int argc, char **argv) {
    int64_t total_size = 0;
    for (int i = 1; i < argc; i++) {
        total_size += file_size(argv[i]);
    }
    printf("%lld", total_size);
    return 0;
}
