#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strfunc.h"
#include "request.h"

#define REQ_DBG 1
#define MAX_FILE_SIZE 2048
#define member_file "member.txt"

/*
 *  Member format
 *  5 lines represents 1 member
 *  Each line in order is id, passwd, name, level, birthday, description
 *  Ex: user.txt
 *      nick0212\n
 *      123456
 *      Nick\n
 *      2\n
 *      1995/02/12\n
 *      He is a good doctor\n
 */

// return line_count
int get_member_lines(char member_lines[][128]) {
    FILE *file = fopen(member_file, "r");
    if (!file) {
        printf("Couldn't find %s\n", member_file);
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

void save_member_lines(char member_lines[][128], int member_line_count) {
    FILE *file = fopen(member_file, "w");
    if (!file) {
        printf("Couldn't find %s\n", member_file);
        return;
    }

    int i;
    for (i = 0; i < member_line_count; i++) {
        fprintf(file, "%s\n", member_lines[i]);
    }
    fclose(file);
}

int get_member_by_id(char *id, char *dst_json) {
    char member_lines[1024][128];
    int member_line_count = get_member_lines(member_lines);

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
    char member_lines[1024][128];
    int member_line_count = get_member_lines(member_lines);

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
    char member_lines_all[1024][128];
    int member_line_count_all = get_member_lines(member_lines_all);

    // Keep level member only
    char member_lines[1024][128];
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
        char each_member[1024];
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

// int add_member(char *name, char *level, char *birthday, char *description, char *ret_id) {
//
// }

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
