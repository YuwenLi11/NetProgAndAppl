#ifndef REQUEST_H
#define REQUEST_H

#define LOGIN_SUCCESS_ADMIN 1
#define LOGIN_SUCCESS_DOCTOR 2
#define LOGIN_SUCCESS_PATIENT 3
#define LOGIN_NOT_EXIST 10
#define LOGIN_INFO_INCORRECT 11

#define COOKIE_USER_ID "HcsUserId="
#define COOKIE_USER_LEVEL "HcsUserLevel="

int get_member_by_id(char *id, char *dst_json);
int login(char *id, char *passwd, char *res);
void logout(char *res);
int load_file_to_buffer(char *file_name, char *buffer);

#endif
