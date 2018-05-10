#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strfunc.h"
#include "request.h"

#define REQ_DBG 1
#define MAX_FILE_SIZE 2048
#define member_file "member.txt"
#define prescription_file "prescription.txt"

/*
 *  Member format
 *  6 lines represents 1 member
 *  Each line in order is id, passwd, name, level, birthday, description
 *  Ex: member.txt
 *      nick0212\n
 *      123456
 *      Nick\n
 *      2\n
 *      1995/02/12\n
 *      He is a good doctor\n
 */

// return line_count
int get_lines(char member_lines[][128], char *file_name) {
    FILE *file = fopen(file_name, "r");
    if (!file) {
        printf("Couldn't find %s\n", file_name);
        return -1;
    }

    char *line = NULL;
    size_t len = 0;
    int line_count = 0;

    while (getline(&line, &len, file) != -1) {
        line[strlen(line) - 1] = '\0'; // remove break line
        strcpy(member_lines[line_count], line);
        line_count++;
    }

    fclose(file);
    return line_count;
}

void save_lines(char member_lines[][128], int member_line_count, char *file_name) {
    FILE *file = fopen(file_name, "w");
    if (!file) {
        printf("Couldn't find %s\n", file_name);
        return;
    }

    int i;
    for (i = 0; i < member_line_count; i++) {
        fprintf(file, "%s\n", member_lines[i]);
    }
    fclose(file);
}

int get_member_by_id(char *id, char *dst_json) {
    char member_lines[256][128];
    int member_line_count = get_lines(member_lines, member_file);

    int i;
    for (i = 0; i < member_line_count; i += 6) {
        if (strcmp(id, member_lines[i]) == 0) { // found
            sprintf(dst_json, "{\"id\": \"%s\", \"name\": \"%s\", \"level\": %s, \"birthday\": \"%s\", \"description\": \"%s\"}",
            member_lines[i], member_lines[i+2], member_lines[i+3], member_lines[i+4], member_lines[i+5]);
            return 1;
        }
    }

    return 0; // not found
}

int get_member_all(char *dst_json) {
    char member_lines[256][128];
    int member_line_count = get_lines(member_lines, member_file);

    if (member_line_count < 6) {
        sprintf(dst_json, "[]");
        return 0;
    }

    // Add first
    sprintf(dst_json, "[\n  {\"id\": \"%s\", \"name\": \"%s\", \"level\": %s, \"birthday\": \"%s\", \"description\": \"%s\"}",
    member_lines[0], member_lines[2], member_lines[3], member_lines[4], member_lines[5]);

    int i;
    for (i = 6; i < member_line_count; i += 6) {
        char each_member[1024];
        sprintf(each_member, ",\n  {\"id\": \"%s\", \"name\": \"%s\", \"level\": %s, \"birthday\": \"%s\", \"description\": \"%s\"}",
        member_lines[i], member_lines[i+2], member_lines[i+3], member_lines[i+4], member_lines[i+5]);

        strcat(dst_json, each_member);
    }

    strcat(dst_json, "\n]");

    return 1;
}

int get_member_template(char *dst_json, char *level) {
    char member_lines_all[256][128];
    int member_line_count_all = get_lines(member_lines_all, member_file);

    // Keep level member only
    char member_lines[256][128];
    int member_line_count = 0;
    int j;
    for (j = 0; j < member_line_count_all; j += 6) {
        if (strcmp(member_lines_all[j + 3], level) == 0) {
            int k;
            for (k = 0; k < 6; k++) {
                strcpy(member_lines[member_line_count + k], member_lines_all[j + k]);
            }
            member_line_count += 6;
        }
    }

    if (member_line_count < 6) {
        sprintf(dst_json, "[]");
        return 0;
    }

    // Add first
    sprintf(dst_json, "[\n  {\"id\": \"%s\", \"name\": \"%s\", \"level\": %s, \"birthday\": \"%s\", \"description\": \"%s\"}",
    member_lines[0], member_lines[2], member_lines[3], member_lines[4], member_lines[5]);

    int i;
    for (i = 6; i < member_line_count; i += 6) {
        char each_member[256];
        sprintf(each_member, ",\n  {\"id\": \"%s\", \"name\": \"%s\", \"level\": %s, \"birthday\": \"%s\", \"description\": \"%s\"}",
        member_lines[i], member_lines[i+2], member_lines[i+3], member_lines[i+4], member_lines[i+5]);

        strcat(dst_json, each_member);
    }

    strcat(dst_json, "\n]");

    return 1;
}

int get_member_admin(char *dst_json) {
    if (REQ_DBG) printf("get_member_admin\n");
    return get_member_template(dst_json, "1");
}

int get_member_doctor(char *dst_json) {
    if (REQ_DBG) printf("get_member_doctor\n");
    return get_member_template(dst_json, "2");
}

int get_member_patient(char *dst_json) {
    if (REQ_DBG) printf("get_member_patient\n");
    return get_member_template(dst_json, "3");
}

int add_member(char *id, char *passwd, char *name, char *level, char *birthday, char *description) {
    char member_lines[256][128];
    int member_line_count = get_lines(member_lines, member_file);

    // check user exist
    int exist = 0;
    int i;
    for (i = 0; i < member_line_count; i += 6) {
        if (strcmp(id, member_lines[i]) == 0) exist = 1;
    }

    if (!exist) {
        strcpy(member_lines[member_line_count], id);
        strcpy(member_lines[member_line_count + 1], passwd);
        strcpy(member_lines[member_line_count + 2], name);
        strcpy(member_lines[member_line_count + 3], level);
        strcpy(member_lines[member_line_count + 4], birthday);
        strcpy(member_lines[member_line_count + 5], description);

        member_line_count += 6;
        save_lines(member_lines, member_line_count, member_file);

        return 1;
    } else {
        return 0;
    }
}

int edit_member(char *id, char *passwd, char *name, char *level, char *birthday, char *description) {
    char member_lines[256][128];
    int member_line_count = get_lines(member_lines, member_file);

    // check user exist
    int i;
    for (i = 0; i < member_line_count; i += 6) {
        if (strcmp(id, member_lines[i]) == 0) { // user exist
            if (id != NULL && strcmp(id, "") != 0) strcpy(member_lines[i], id);
            if (id != NULL && strcmp(passwd, "") != 0) strcpy(member_lines[i + 1], passwd);
            if (id != NULL && strcmp(name, "") != 0) strcpy(member_lines[i + 2], name);
            if (id != NULL && strcmp(level, "") != 0) strcpy(member_lines[i + 3], level);
            if (id != NULL && strcmp(birthday, "") != 0) strcpy(member_lines[i + 4], birthday);
            if (id != NULL && strcmp(description, "") != 0) strcpy(member_lines[i + 5], description);

            save_lines(member_lines, member_line_count, member_file);
            return 1;
        }
    }

    return 0; // not found
}

int delete_member(char *id) {
  char member_lines[256][128];
  int member_line_count = get_lines(member_lines, member_file);

  // check user exist
  int i;
  for (i = 0; i < member_line_count; i += 6) {
      if (strcmp(id, member_lines[i]) == 0) { // user exist
          if (i + 6 < member_line_count) { // not the last 6 lines
              // replace the last 6 lines to these 6 lines
              strcpy(member_lines[i], member_lines[member_line_count - 6]);
              strcpy(member_lines[i + 1], member_lines[member_line_count - 5]);
              strcpy(member_lines[i + 2], member_lines[member_line_count - 4]);
              strcpy(member_lines[i + 3], member_lines[member_line_count - 3]);
              strcpy(member_lines[i + 4], member_lines[member_line_count - 2]);
              strcpy(member_lines[i + 5], member_lines[member_line_count - 1]);
          }

          save_lines(member_lines, member_line_count - 6, member_file);
          return 1;
      }
  }

  return 0; // not found
}

/*
 *  Prescription format
 *  5 lines represents 1 member
 *  Each line in order is id, doctor_id, patient_id, date, prescription
 *  Ex: prescription.txt
 *      1\n
 *      thedoctor\n
 *      thepatient\n
 *      2018/05/12\n
 *      The disease is ...\n
 */

int get_prescription_by_id(char *id, char *dst_json) {
    char member_lines[256][128];
    int member_line_count = get_lines(member_lines, prescription_file);

    int i;
    for (i = 0; i < member_line_count; i += 5) {
        if (strcmp(id, member_lines[i]) == 0) {
            sprintf(dst_json, "{\"id\": \"%s\", \"doctor_id\": \"%s\", \"patient_id\": %s, \"date\": \"%s\", \"prescription\": \"%s\"}",
            member_lines[i], member_lines[i+1], member_lines[i+2], member_lines[i+3], member_lines[i+4]);
            return 1;
        }
    }

    return 0; // not found
}

int get_prescription_all(char *dst_json) {
    char member_lines[256][128];
    int member_line_count = get_lines(member_lines, prescription_file);

    if (member_line_count < 5) {
        sprintf(dst_json, "[]");
        return 0;
    }

    // Add first
    sprintf(dst_json, "[\n  {\"id\": \"%s\", \"doctor_id\": \"%s\", \"patient_id\": %s, \"date\": \"%s\", \"prescription\": \"%s\"}",
    member_lines[0], member_lines[1], member_lines[2], member_lines[3], member_lines[4]);

    int i;
    for (i = 5; i < member_line_count; i += 5) {
        char each_member[1024];
        sprintf(each_member, ",\n  {\"id\": \"%s\", \"doctor_id\": \"%s\", \"patient_id\": %s, \"date\": \"%s\", \"prescription\": \"%s\"}",
        member_lines[i], member_lines[i+1], member_lines[i+2], member_lines[i+3], member_lines[i+4]);

        strcat(dst_json, each_member);
    }

    strcat(dst_json, "\n]");

    return 1;
}

int add_prescription(char *doctor_id, char *patient_id, char *date, char *prescription) {
    char member_lines[256][128];
    int member_line_count = get_lines(member_lines, prescription_file);

    // find max id
    int max_id = atoi(member_lines[member_line_count - 5]);

    sprintf(member_lines[member_line_count], "%d", max_id + 1);
    strcpy(member_lines[member_line_count + 1], doctor_id);
    strcpy(member_lines[member_line_count + 2], patient_id);
    strcpy(member_lines[member_line_count + 3], date);
    strcpy(member_lines[member_line_count + 4], prescription);

    member_line_count += 5;
    save_lines(member_lines, member_line_count, prescription_file);

    return 1;

}

int edit_prescription(char *id, char *doctor_id, char *patient_id, char *date, char *prescription);

int delete_prescription(char *id);

int login(char *id, char *passwd, char *res) {
    // validation
    int valid = LOGIN_NOT_EXIST;
    if (REQ_DBG) {
        if (strcmp(id, "testadmin") == 0) {
            valid = LOGIN_SUCCESS_ADMIN;
        } else if (strcmp(id, "testdoctor") == 0) {
            valid = LOGIN_SUCCESS_DOCTOR;
        } else if (strcmp(id, "testpatient") == 0) {
            valid = LOGIN_SUCCESS_PATIENT;
        }
    }

    switch (valid) {
        case LOGIN_SUCCESS_ADMIN:
            if (REQ_DBG) printf("LOGIN_SUCCESS_ADMIN\n");
            sprintf(res, "HTTP/1.1 200 OK\r\nSet-Cookie: %s%s\r\nSet-Cookie: %s%d\r\n\r\n", COOKIE_USER_ID, id, COOKIE_USER_LEVEL, LOGIN_SUCCESS_ADMIN);
            break;
        case LOGIN_SUCCESS_DOCTOR:
            if (REQ_DBG) printf("LOGIN_SUCCESS_DOCTOR\n");
            sprintf(res, "HTTP/1.1 200 OK\r\nSet-Cookie: %s%s\r\nSet-Cookie: %s%d\r\n\r\n", COOKIE_USER_ID, id, COOKIE_USER_LEVEL, LOGIN_SUCCESS_DOCTOR);
            break;
        case LOGIN_SUCCESS_PATIENT:
            if (REQ_DBG) printf("LOGIN_SUCCESS_PATIENT\n");
            sprintf(res, "HTTP/1.1 200 OK\r\nSet-Cookie: %s%s\r\nSet-Cookie: %s%d\r\n\r\n", COOKIE_USER_ID, id, COOKIE_USER_LEVEL, LOGIN_SUCCESS_PATIENT);
            break;
        case LOGIN_NOT_EXIST:
        case LOGIN_INFO_INCORRECT:
            sprintf(res, "HTTP/1.1 403 Forbidden\r\nSet-Cookie: %s\"\"\r\n\r\n", COOKIE_USER_ID);
    }

    return valid;
}

void logout(char *res) {
    sprintf(res, "HTTP/1.1 200 OK\r\nSet-Cookie: %s\"\"\r\n\r\n", COOKIE_USER_ID);
}

/************************************************************
 * Function: load_file_to_buffer
 *   Read file by a given name
 * Parameters:
 *   file_name - file name, in the same path that server is executed
 *   buffer - the buffer that file content will put into
 * Returns:
 *   >= 0 - file size
 *   -1 - error
 ************************************************************/
int load_file_to_buffer(char *file_name, char *buffer) {
    FILE *file;

    file = fopen(file_name, "rb");
    if (!file) {
        printf("Couldn't find %s\n", file_name);
        return -1;
    }

    fread(buffer, MAX_FILE_SIZE, 1, file);
    fseek(file, 0L, SEEK_END); // in order to find file size
    int file_size = ftell(file);

    printf("Server read %s, size %d bytes\n", file_name, file_size);

    fclose(file);
    return file_size;
}
