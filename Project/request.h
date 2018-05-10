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
int get_member_all(char *dst_json);
int get_member_admin(char *dst_json);
int get_member_doctor(char *dst_json);
int get_member_patient(char *dst_json);
int add_member(char *id, char *passwd, char *name, char *level, char *birthday, char *description);
int edit_member(char *id, char *passwd, char *name, char *level, char *birthday, char *description);
int delete_member(char *id);
int get_prescription_by_id(char *id, char *dst_json);
int get_prescription_all(char *dst_json);
int add_prescription(char *doctor_id, char *patient_id, char *date, char *prescription);
int edit_prescription(char *id, char *doctor_id, char *patient_id, char *date, char *prescription);
int delete_prescription(char *id);
int login(char *id, char *passwd, char *res);
void logout(char *res);
int load_file_to_buffer(char *file_name, char *buffer);

#endif
