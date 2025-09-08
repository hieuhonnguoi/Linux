#include "strutils.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

void str_reverse(char *str) {
    if (!str) return;
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char tmp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = tmp;
    }
}

void str_trim(char *str) {
    if (!str) return;

    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    char *end = str + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1))) {
        end--;
    }

    *end = '\0'; // Kết thúc chuỗi

    if (start != str) {
        memmove(str, start, end - start + 1);
    }
}

int str_to_int(const char *str, int *out) {
    if (!str || !out) return 0;

    char *endptr;
    long val = strtol(str, &endptr, 10);

    if (*endptr != '\0') {
        return 0;
    }

    *out = (int)val;
    return 1;
}
