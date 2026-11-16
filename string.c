#include "string.h"

int stringcmp(char *a, char *b)
{
    while (*a && *b) {
        if (*a != *b)
            return 0;

        a++;
        b++;
    }

    return (*a == '\0' && *b == '\0');
}