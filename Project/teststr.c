#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strfunc.h"

int main() {
    char *src = "  \nabc\r\ndef\r\n";
    printf("%s\n", src);

    char dst[128] = {0};
    int new_pos = 0;
    while (new_pos < strlen(src)) {
        new_pos = get_str_line(src, new_pos, dst);
        printf("%s, %d\n", dst, new_pos);
    }
    return 0;
}
