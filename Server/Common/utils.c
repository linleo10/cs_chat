#include "utils.h"

void trimString(char *str) {
    int start = 0, end = strlen(str) - 1;

    while (start <= end && isspace(str[start])) {
        start++;
    }

    while (end >= start && isspace(str[end])) {
        end--;
    }

    int len = end - start + 1;
    memmove(str, str + start, len);
    str[len] = '\0';
}