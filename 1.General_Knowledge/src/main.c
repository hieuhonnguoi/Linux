#include <stdio.h>
#include <string.h>
#include "strutils.h"

int main() {
    char s[100];  

    strcpy(s, "Hello World!");
    str_reverse(s);
    printf("Reversed: '%s'\n", s);


    strcpy(s, "   Hello World   ");
    str_trim(s);
    printf("Trimmed: '%s'\n", s);


    strcpy(s, "12345");
    int number;
    if (str_to_int(s, &number)) {
        printf("Converted: %d\n", number);
    } else {
        printf("Invalid integer string: '%s'\n", s);
    }

    strcpy(s, "12a34");
    if (str_to_int(s, &number)) {
        printf("Converted: %d\n", number);
    } else {
        printf("Invalid integer string: '%s'\n", s);
    }

    return 0;
}
