#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#define BUFF_SIZE 128

#define LINE_SIZE (100 + 1)

time_t parse(char *line) {
    int year, mon, mday, hour, min, sec;
    struct tm tm_object = {0};
    if (sscanf(line, "%d/%d/%d %d:%d:%d", &year, &mon, &mday, &hour, &min, &sec) != 6) {
        return -1;
    }
    int isdst = -1;
    year -= 1900;
    mon -= 1;
    tm_object.tm_year = year;
    tm_object.tm_mon = mon;
    tm_object.tm_mday = mday;
    tm_object.tm_hour = hour;
    tm_object.tm_min = min;
    tm_object.tm_sec = sec;
    tm_object.tm_isdst = isdst;

    return mktime(&tm_object);
}
int main(int argc, char **argv) {

    if (argc < 2) {
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        return 1;
    }

    char buff[BUFF_SIZE];
    ssize_t bytes_read;
    char line[LINE_SIZE];
    int line_pos = 0;

    time_t prev_timestamp = -1;

    while ((bytes_read = read(fd, buff, BUFF_SIZE)) > 0) {
        for (int i = 0; i < bytes_read; i++) {

            if (buff[i] == '\n') {
                line[line_pos] = '\0';
                time_t current_timestamp = parse(line);
                if (prev_timestamp != -1 && current_timestamp != -1) {
                    printf("%lld\n", (long long) current_timestamp - prev_timestamp);
                }

                prev_timestamp = current_timestamp;
                line_pos = 0;

            } else {
                if (line_pos < LINE_SIZE - 1) {
                    line[line_pos++] = buff[i];
                }
            }
        }
    }
    if (bytes_read == -1) {
        close(fd);
        return 1;
    }

    if (line_pos > 0 && prev_timestamp != -1) {
        line[line_pos] = '\0';
        time_t current_timestamp = parse(line);
        printf("%lld\n", (long long) current_timestamp - prev_timestamp);
    }

    if (close(fd) == -1) {
        return 1;
    }
    return 0;
}
