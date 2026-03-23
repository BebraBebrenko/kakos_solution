#include <errno.h>
#include <stdio.h>
#include <time.h>

#define NEXT_DAY                                                                                                       \
    tm_object.tm_mday++;                                                                                               \
    if (timegm(&tm_object) == -1 && errno != 0) {                                                                      \
        return 1;                                                                                                      \
    }

#define NEXT_WEEK                                                                                                      \
    thursday_count++;                                                                                                  \
    tm_object.tm_mday += 7;                                                                                            \
    if (timegm(&tm_object) == -1 && errno != 0) {                                                                      \
        return 1;                                                                                                      \
    }
// Source - https://stackoverflow.com/a/11763277
// Posted by netcoder, modified by community. See post 'Timeline' for change history
// Retrieved 2026-02-11, License - CC BY-SA 4.0

#define FILL_1(name) tm_object.tm_##name = name;
#define FILL_2(name, val) tm_object.tm_##name = val;
#define GET_MACRO(_1, _2, NAME, ...) NAME
#define fill(...) GET_MACRO(__VA_ARGS__, FILL_2, FILL_1)(__VA_ARGS__)

int main(int argc, char **argv) {
    int year = 0;
    int current_month = 0;
    if (!scanf("%d", &year)) {
        return 1;
    }
    year -= 1900;
    struct tm tm_object = {0};
    fill(year);

    fill(mon, 0);
    fill(mday, 1);
    fill(hour, 0);
    fill(min, 0);
    fill(sec, 0);
    fill(isdst, -1);

    timegm(&tm_object);
    while (tm_object.tm_wday != 4) {
        NEXT_DAY
    }
    while (tm_object.tm_year == year) {
        int thursday_count = 1;
        while (tm_object.tm_mon == current_month) {
            if ((thursday_count == 2 || thursday_count == 4) && tm_object.tm_mday % 3 != 0) {
                printf("%d %d\n", tm_object.tm_mon + 1, tm_object.tm_mday);
            }
            NEXT_WEEK
        }
        current_month++;
    }
    return 0;
}
