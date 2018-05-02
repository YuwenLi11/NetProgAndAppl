#include <stdio.h>
#include <string.h>
#include "strfunc.h"
#include "request.h"

#define REQ_DBG 1

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
            sprintf(res, "HTTP/1.1 200 OK\r\nSet-Cookie: %s\"\"\r\n\r\n", COOKIE_USER_ID);
    }

    return valid;
}

void logout(char *res) {
    sprintf(res, "HTTP/1.1 200 OK\r\nSet-Cookie: %s\"\"\r\n\r\n", COOKIE_USER_ID);
}
