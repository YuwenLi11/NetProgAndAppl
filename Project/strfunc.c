#include "strfunc.h"
#include <string.h>

/************************************************************
 * Function: compare_str
 *   Compare two string with different start position by given length
 *   EX - compare_str("abc", 1, "bcd", 0, 2) return 0, since "bc" equals to "bc"
 * Parameters:
 *   str1, str2 - two strings
 *   start1, start2 - start position of the string
 *   len - the length to be compare from two start position
 * Returns:
 *   0 - two string are the same, with the given parameters
 *   1 - two string are not the same, with the given parameters
 ************************************************************/
int compare_str(char *str1, int start1, char *str2, int start2, int len) {
    // check corner case
    if (start1 + len > strlen(str1) || start2 + len > strlen(str2)) return 1;

    int cnt = 0;
    while (start1 < strlen(str1) && start2 < strlen(str2) && cnt < len) {
        if (str1[start1++] != str2[start2++]) {
            return 1; // not the same
        }
        cnt++;
    }
    return 0; // the same
}

/************************************************************
 * Function: get_str_until_space
 *   Get substr of src from given position until the first space and put in dst
 *   EX - get_str_until_space("abc def", 0, dst) return 3, dst will be "abc"
 * Parameters:
 *   src - original string
 *   src_start - the start position (including) of src
 *   dst - the buffer that the substring will be put into
 * Returns:
 *   the position of next space or the length of src (it traverses to the end)
 ************************************************************/
int get_str_until_space(char *src, int src_start, char *dst) {
    // skip space at the beginning
    while (src[src_start] == ' ') {
      src_start++;
    }

    // start to get string
    int i;
    for (i = 0; src_start < strlen(src) && src[src_start] != ' '; i++) {
        dst[i] = src[src_start];
        src_start++;
    }
    dst[i] = '\0';
    return src_start;
}

/************************************************************
 * Function: get_str_line
 *   Get substr of src from given position until end of line and put in dst
 *   EX - get_str_line(" \nabc\r\ndef", 0, dst) return 5, dst will be "abc"
 * Parameters:
 *   src - original string
 *   src_start - the start position (including) of src
 *   dst - the buffer that the substring will be put into
 * Returns:
 *   the position of next \n, \r or space or the length or src
 ************************************************************/
int get_str_line(char *src, int src_start, char *dst) {
    dst[0] = '\0'; // clean dst

    // remove \r, \n and space from the beginning
    int i = src_start;
    while (i < strlen(src)) {
        if (src[i] == '\r' || src[i] == '\n' || src[i] == ' ') i++;
        else break;
    }

    int cnt = 0;
    while (i < strlen(src) && src[i] != '\r' && src[i] != '\n') {
        dst[cnt++] = src[i++];
    }

    return i;
}

/************************************************************
 * Function: strcpy_with_pos_len
 *   Copy string from src to dst by given start position and length
 * Parameters:
 *   src - original string
 *   src_start - the start position (including) of src
 *   src_len - the length to be copied
 *   dst - the buffer that the substring will be put into
 ************************************************************/
void strcpy_with_pos_len(char *src, int src_start, int src_len, char *dst) {
    int i;
    for (i = 0; i < strlen(src) && i <= src_len; i++) {
        dst[i] = src[i + src_start];
    }
    dst[i] = '\0';
}
