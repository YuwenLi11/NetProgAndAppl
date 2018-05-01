#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strfunc.h"

int main() {
    char dst[128] = {0};
    int new_pos = 0;

    // get_str_line test
    char *src1 = "  \nabc\r\ndef\r\n";
    printf("src1: %s\n", src1);

    printf("get_str_line test\n");
    while (new_pos < strlen(src1)) {
        new_pos = get_str_line(src1, new_pos, dst);
        printf("%s, %d\n", dst, new_pos);
    }

    // get_str_until_space test
    char *src2 = "  abc  def gh";
    printf("src2: %s\n", src2);

    new_pos = 0;
    printf("get_str_until_space test\n");
    while (new_pos < strlen(src2)) {
        new_pos = get_str_until_space(src2, new_pos, dst);
        printf("%s, %d\n", dst, new_pos);
    }

    // strcpy_with_pos_len test
    char *src3 = "abcde";
    char dst3[32];
    printf("strcpy_with_pos_len\n");
    strcpy_with_pos_len(src3, 1, 2, dst3);
    printf("%s\n", dst3);

    // get_from_two_str test
    char *src4 = "1234567890";
    char dst4[32];
    printf("get_from_two_str\n");
    printf("find 345 789 from %s\n", src4);
    int ans = get_from_two_str(src4, "345", "7890", dst4);
    printf("Ans: %d, %s\n", ans, dst4);
    printf("find 432 789 from %s\n", src4);
    ans = get_from_two_str(src4, "543", "789", dst4);
    printf("Ans: %d, %s\n", ans, dst4);


    return 0;
}
