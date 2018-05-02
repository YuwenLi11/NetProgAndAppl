#include <stdio.h>
#include <string.h>
#include "strfunc.h"
#include "request.h"

#define REQ_DBG 1

int login(char *id, char *passwd, char *res) {
    // validation
    int valid = 0;
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
        case LOGIN_SUCCESS_DOCTOR:
        case LOGIN_SUCCESS_PATIENT:
            sprintf(res, "HTTP/1.1 200 OK\r\nSet-Cookie: %s%s; Set-Cookie: %s%d\r\n\r\n", COOKIE_USER_ID, id, COOKIE_USER_LEVEL, LOGIN_SUCCESS_ADMIN);
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
