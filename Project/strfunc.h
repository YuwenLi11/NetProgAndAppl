#ifndef STRFUNC_H
#define STRFUNC_H

int compare_str(char *str1, int start1, char *str2, int start2, int len);
int get_str_until_space(char *src, int src_start, char *dst);
int get_restful_route(char *route, char dst[][32]);
int get_str_line(char *src, int src_start, char *dst);
void strcpy_with_pos_len(char *src, int src_start, int src_len, char *dst);
int get_from_two_str(char *src, char *str1, char *str2, char *dst);
int get_json_val_by_key(char *header, char *key, char *dst);

#endif
