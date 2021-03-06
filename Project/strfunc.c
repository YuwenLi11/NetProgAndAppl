#include <stdio.h>
#include <string.h>
#include "strfunc.h"
#include "jsmn.h"

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
 * Function: get_restful_route
 *   Separate restful route into 2-d array
 *   Ex: route: /api/member/add -> *dst[]: ["api", "member", "add"] and return 3
 * Parameters:
 *   route - restful route
 *   dst - 2-d array to be put the result
 * Returns:
 *   the number of dst array
 *   -1 - error
 ************************************************************/
int get_restful_route(char *route, char dst[][32]) {
    if (strlen(route) == 0) {
        printf("[Error] get_restful_route, no input\n");
        return -1;
    } else if (route[0] != '/') {
      printf("[Error] get_restful_route, syntax error\n");
        return -1;
    }

    int i = 1;
    int count = 0;
    while (i < strlen(route)) {
        int j;
        for (j = 0; i + j < strlen(route) && route[i + j] != '/'; j++) {
            dst[count][j] = route[i + j];
        }
        dst[count][j] = '\0';
        i += j + 1; // skip slash
        count++;
    }

    return count;
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
    for (i = 0; i < strlen(src) && i < src_len; i++) {
        dst[i] = src[i + src_start];
    }
    dst[i] = '\0';
}

/************************************************************
 * Function: get_start_from_str
 *   Find the start position of str in src
 * Parameters:
 *   src - original string
 *   start - the start position (including) of src
 *   str - the string to be found within src
 * Returns:
 *   the start position of str in src
 ************************************************************/
int get_start_from_str(char *src, int start, char *str) {
    int f = start, s = start; // fast, slow pointer, find word between [s, f)
    // find str
    while (f++ < strlen(src)) {
        if (src[f] == str[0]) {
            s = f;
            int i;
            for (i = 0; i < strlen(str) && src[f++] == str[i]; i++);
            if (i == strlen(str)) { // match str
                return s;
            }
        }
    }
    return -1;
}

/************************************************************
 * Function: get_from_two_str
 *   Get the content from two str and put it into dst
 * Parameters:
 *   src - original string
 *   str1 - first string to be found within src
 *   str2 - second string to be found within src,
            empty string as input means get the string to the end
 *   dst - the buffer that the content between str1, str2 will be put into
 * Returns:
 *   0 - not found
 *   1 - found
 ************************************************************/
int get_from_two_str(char *src, char *str1, char *str2, char *dst) {
    int start1 = get_start_from_str(src, 0, str1);
    if (start1 == -1) return 0; // not found

    int content_start = start1 + strlen(str1);

    int start2 = strlen(src); // special case, get string to the end
    if (strlen(str2) != 0) {
      start2 = get_start_from_str(src, content_start, str2);
      if (start2 == -1) return 0; // not found
    }

    // assign string to dst
    strcpy_with_pos_len(src, content_start, start2 - content_start, dst);
    return 1;
}


static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

/************************************************************
 * Function: get_json_val_by_key
 *   Get val by key from a json in a header
 * Parameters:
 *   header - client header string
 *   key - json key
 *   dst - json val to be put
 * Returns:
 *   0 - not found
 *   1 - found
 ************************************************************/
int get_json_val_by_key(char *header, char *key, char *dst) {
    char json_str_buf[1024], json_str[1024];
    int errcode = get_from_two_str(header, "{", "}", json_str_buf);
    if (errcode == 0) return 0; // not found json format string
    sprintf(json_str, "{%s}", json_str_buf);

    int i;
    int r;
    jsmn_parser p;
    jsmntok_t t[128]; // We expect no more than 128 tokens

    jsmn_init(&p);
    r = jsmn_parse(&p, json_str, strlen(json_str), t, sizeof(t)/sizeof(t[0]));
    if (r < 0) {
        printf("Failed to parse JSON: %d\n", r);
        return 0;
    }

    // Assume the top-level element is an object
    if (r < 1 || t[0].type != JSMN_OBJECT) {
        printf("Object expected\n");
        return 0;
    }

    // find key
    for (i = 1; i < r; i++) {
    		if (jsoneq(json_str, &t[i], key) == 0) {
      			/* We may use strndup() to fetch string value */
      			sprintf(dst, "%.*s", t[i+1].end-t[i+1].start,
      					json_str + t[i+1].start);
      			return 1;
    		}
  	}

    return 0; // successful
}
