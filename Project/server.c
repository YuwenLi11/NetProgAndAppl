#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>         // string manipulation
#include <sys/socket.h>
#include <unistd.h>         // read, write

#include "strfunc.h"
#include "request.h"

#define BACKLOG 10 // queue length
#define MAX_HEADER_SIZE 65536
#define HTML_FOLDER "html"

#define DBG 1

struct client_param {
    int client_sd;
    char client_ip[32];
};

int start_socket(int port);
void *conn_handler(void *param);
void get_response(char *res, char *client_header);
int load_file_to_buffer(char *file_name, char *buffer);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Port is required\n");
        exit(-1);
    }
    int port = atoi(argv[1]);
    int sd = start_socket(port);

    // Define client variables
    struct sockaddr_in client;
    socklen_t client_len = sizeof(struct sockaddr_in);

    while (1) {
        // Accept
        int client_sd = accept(sd, (struct sockaddr *)&client, &client_len);
        char client_ip[32] = {0};
        if (client_sd < 0) {
            printf("Accept failed\n");
            exit(-1);
        }
        // Format client ip address and port into client_ip
        sprintf(client_ip, "%s:%d", inet_ntoa(client.sin_addr), client.sin_port);
        printf("Connection accepted with client %s\n", client_ip);

        pthread_t conn_thread;
        struct client_param param;
        param.client_sd = client_sd;
        strcpy(param.client_ip, client_ip);
        if (pthread_create(&conn_thread, NULL, conn_handler, &param) < 0) {
            printf("Create thread failed");
            exit(-1);
        }
        sleep(1); // prevent threading problem
    }

    return 0;
}

/************************************************************
 * Function: start_socket
 *   Create a socket, bind and listen
 * Parameters:
 *   port: listening port
 * Returns:
 *   sd (socket descriptor)
 ************************************************************/
int start_socket(int port) {
    // Create socket
    int sd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM for TCP
    if (sd < 1) {
        printf("Couldn't create socket\n");
        exit(-1);
    }
    if (DBG) printf("Socket created, sd = %d\n", sd);

    // Set server configuration
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; // enable any ip_addr
    server.sin_port = htons(port);

    // Bind
    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Bind failed\n");
        exit(-1);
    }
    if (DBG) printf("Bind sd(%d) successful\n", sd);

    // Listen
    if (listen(sd, BACKLOG) < 0) {
        printf("Listen failed\n");
        exit(-1);
    }
    if (DBG) printf("Listen successful\n");
    printf("HTTP Server running on port %d\n", port);

    return sd;
}

/************************************************************
 * Function: conn_handler
 *   Thread executing function, read and write with client by given client_sd
 * Parameters:
 *   void *param - a client_param structure
 * Returns:
 *   0 - successful
 ************************************************************/
void *conn_handler(void *param) {
    if (DBG) printf("New thread created\n");
    struct client_param *cp = (struct client_param *)param;
    int client_sd = cp->client_sd;
    char* client_ip = cp->client_ip;

    // Read
    char client_header[MAX_HEADER_SIZE] = {0};
    if (DBG) printf("Ready to read from %s\n", client_ip);
    int read_size = read(client_sd, client_header, MAX_HEADER_SIZE);
    if (read_size < 0) {
        printf("Read failed\n");
        exit(-1);
    }
    if (DBG) {
        printf("=== Read successful, client header ===\n");
        printf("%s", client_header);
        printf("======================================\n");
    }

    // Write
    // Define response
    char res[MAX_HEADER_SIZE] = {0};
    get_response(res, client_header);
    if (write(client_sd, res, strlen(res)) < 0) {
        printf("Write failed\n");
        exit(-1);
    }
    if (DBG) {
        printf("DBG - Write successful, respond to %s\n", client_ip);
        printf("DBG - Write content: \n%s\n\n", res);
    }
    close(client_sd);
    return 0;
}

void get_response(char *res, char *client_header) {
    // Parsing client header
    char line[32][128];
    int i = 0, line_cnt = 0;
    while (i < strlen(client_header)) {
        i = get_str_line(client_header, i, line[line_cnt]);
        line_cnt++;
    }
    if (DBG) {
        printf("Header each line:\n");
        for (int j = 0; j < line_cnt; j++) {
            printf("DBG - Each line: %s\n", line[j]);
        }
    }

    // Determine method
    int cursor = 0;
    char method[8], request_route[32];
    cursor = get_str_until_space(line[0], cursor, method);
    printf("Method = %s\n", method);
    cursor = get_str_until_space(line[0], cursor, request_route);
    printf("Request Route = %s\n", request_route);

    // Get UserId
    char userid[32];
    int errcode = get_from_two_str(client_header, COOKIE_USER_ID, "\n", userid);
    if (errcode != 0) {
        printf("%s%s\n", COOKIE_USER_ID, userid);
    } else {
        printf("Couldn't find %s\n", COOKIE_USER_ID);
    }

    // Separate request_route
    char routes[16][32];
    int route_count = get_restful_route(request_route, routes);

    // Determine response file path
    if (strcmp(method, "GET") == 0) {
        printf("GET: %s\n", request_route);

        if (strcmp(routes[0], "api") == 0) {
            if (strcmp(routes[1], "member") == 0) {
                if (strcmp(routes[2], "get") == 0) {
                    if (strcmp(routes[3], "id") == 0) {
                        if (route_count == 5) { // id: fifth param
                            printf("[Action] /api/member/get/id : %s\n", routes[4]);
                            char member_info[1024];
                            int found = get_member_by_id(routes[4], member_info);
                            if (found) sprintf(res, "HTTP/1.1 200 OK\r\n\r\n%s\r\n", member_info);
                            else sprintf(res, "HTTP/1.1 200 OK\r\n\r\n{}\r\n");
                        } else {
                            // no id or more param
                            strcpy(res, "HTTP/1.1 404 Not Found\r\n\r\n");
                        }
                    } else if (strcmp(routes[3], "all") == 0 && route_count == 4) {
                        printf("[Action] /api/member/get/all\n");
                        char members_info[5120];
                        get_member_all(members_info);
                        sprintf(res, "HTTP/1.1 200 OK\r\n\r\n%s\r\n", members_info);
                    } else if (strcmp(routes[3], "admin") == 0 && route_count == 4) {
                        printf("[Action] /api/member/get/admin\n");
                        char members_info[5120];
                        get_member_admin(members_info);
                        sprintf(res, "HTTP/1.1 200 OK\r\n\r\n%s\r\n", members_info);
                    } else if (strcmp(routes[3], "doctor") == 0 && route_count == 4) {
                        char members_info[5120];
                        get_member_doctor(members_info);
                        sprintf(res, "HTTP/1.1 200 OK\r\n\r\n%s\r\n", members_info);
                        printf("[Action] /api/member/get/doctor\n");
                    } else if (strcmp(routes[3], "patient") == 0 && route_count == 4) {
                        char members_info[5120];
                        get_member_patient(members_info);
                        sprintf(res, "HTTP/1.1 200 OK\r\n\r\n%s\r\n", members_info);
                        printf("[Action] /api/member/get/patient\n");
                    }
                } else {
                    // /api/member/?
                    strcpy(res, "HTTP/1.1 404 Not Found\r\n\r\n");
                }
            } else if (strcmp(routes[1], "prescription") == 0) {
              if (strcmp(routes[2], "get") == 0) {
                  if (strcmp(routes[3], "id") == 0) {
                      if (route_count == 5) { // id: fifth param
                          printf("[Action] /api/prescription/get/id : %s\n", routes[4]);
                      } else {
                          // no id or more param
                          strcpy(res, "HTTP/1.1 404 Not Found\r\n\r\n");
                      }
                  } else if (strcmp(routes[3], "all") == 0 && route_count == 4) {
                      printf("[Action] /api/prescription/get/all\n");
                  } else {
                      // /api/prescription/get/?
                      strcpy(res, "HTTP/1.1 404 Not Found\r\n\r\n");
                  }
              } else {
                  // /api/prescription/?
                  strcpy(res, "HTTP/1.1 404 Not Found\r\n\r\n");
              }
            } else {
                // /api/?
                strcpy(res, "HTTP/1.1 404 Not Found\r\n\r\n");
            }
        } else {
            // /?
            strcpy(res, "HTTP/1.1 404 Not Found\r\n\r\n");
        }
    } else if(strcmp(method, "POST") == 0) {
        printf("POST: %s\n", request_route);

        if (strcmp(routes[0], "api") == 0) {
            if (strcmp(routes[1], "member") == 0) {
                char id[16], passwd[16], name[32], level[4], birthday[16], description[256];
                if (strcmp(routes[2], "add") == 0 && route_count == 3) {
                    get_json_val_by_key(client_header, "id", id);
                    printf("[Action] /api/member/add id:%s\n", id);
                    get_json_val_by_key(client_header, "passwd", passwd);
                    get_json_val_by_key(client_header, "name", name);
                    get_json_val_by_key(client_header, "level", level);
                    get_json_val_by_key(client_header, "birthday", birthday);
                    get_json_val_by_key(client_header, "description", description);
                    if (DBG) printf("DBG - member add id(%s) passwd(%s) name(%s) level(%s) birthday(%s) description(%s)\n", id, passwd, name, level, birthday, description);

                    // Fetch data
                    int success = add_member(id, passwd, name, level, birthday, description);
                    if (success) sprintf(res, "HTTP/1.1 200 OK\r\n\r\n{\"success\": true}\r\n");
                    else sprintf(res, "HTTP/1.1 200 OK\r\n\r\n{\"success\": false}\r\n");

                } else if (strcmp(routes[2], "edit") == 0 && route_count == 3) {
                    get_json_val_by_key(client_header, "id", id);
                    printf("[Action] /api/member/edit id:%s\n", id);
                    get_json_val_by_key(client_header, "passwd", passwd);
                    get_json_val_by_key(client_header, "name", name);
                    get_json_val_by_key(client_header, "level", level);
                    get_json_val_by_key(client_header, "birthday", birthday);
                    get_json_val_by_key(client_header, "description", description);
                    if (DBG) printf("DBG - member edit id(%s) passwd(%s) name(%s) level(%s) birthday(%s) description(%s)\n", id, passwd, name, level, birthday, description);

                    // Fetch data
                    int success = edit_member(id, passwd, name, level, birthday, description);
                    if (success) sprintf(res, "HTTP/1.1 200 OK\r\n\r\n{\"success\": true}\r\n");
                    else sprintf(res, "HTTP/1.1 200 OK\r\n\r\n{\"success\": false}\r\n");
                } else if (strcmp(routes[2], "delete") == 0 && route_count == 3) {
                    get_json_val_by_key(client_header, "id", id);
                    printf("[Action] /api/member/delete id:%s\n", id);

                    // Fetch data
                    int success = delete_member(id);
                    if (success) sprintf(res, "HTTP/1.1 200 OK\r\n\r\n{\"success\": true}\r\n");
                    else sprintf(res, "HTTP/1.1 200 OK\r\n\r\n{\"success\": false}\r\n");
                } else {
                    // /api/member/?
                    strcpy(res, "HTTP/1.1 404 Not Found\r\n\r\n");
                }
            } else if (strcmp(routes[1], "login") == 0) {
                char id[32], passwd[32];
                get_json_val_by_key(client_header, "id", id);
                printf("[Action] Login id:%s\n", id);
                login(id, passwd, res);
            } else if (strcmp(routes[1], "logout") == 0) {
                printf("[Action] Logout\n");
                logout(res);
            } else if (strcmp(routes[1], "prescription") == 0) {
                char id[16], doctor_id[16], patient_id[16], date[16], prescription[256];
                if (strcmp(routes[2], "add") == 0 && route_count == 3) {
                    printf("[Action] /api/prescription/add\n");
                    get_json_val_by_key(client_header, "doctor_id", doctor_id);
                    get_json_val_by_key(client_header, "patient_id", patient_id);
                    get_json_val_by_key(client_header, "date", date);
                    get_json_val_by_key(client_header, "prescription", prescription);
                    if (DBG) printf("DBG - prescription add doctor_id(%s) patient_id(%s) date(%s) prescription(%s)\n", doctor_id, patient_id, date, prescription);
                    // TODO: DB
                } else if (strcmp(routes[2], "edit") == 0 && route_count == 3) {
                    get_json_val_by_key(client_header, "id", id);
                    printf("[Action] /api/prescription/edit id:%s\n", id);
                    get_json_val_by_key(client_header, "doctor_id", doctor_id);
                    get_json_val_by_key(client_header, "patient_id", patient_id);
                    get_json_val_by_key(client_header, "date", date);
                    get_json_val_by_key(client_header, "prescription", prescription);
                    if (DBG) printf("DBG - prescription edit id(%s) doctor_id(%s) patient_id(%s) date(%s) prescription(%s)\n", id, doctor_id, patient_id, date, prescription);
                    // TODO: DB
                } else if (strcmp(routes[2], "delete") == 0 && route_count == 3) {
                    get_json_val_by_key(client_header, "id", id);
                    printf("[Action] /api/prescription/delete id:%s\n", id);
                    // TODO: DB
                } else {
                    // /api/prescription/?
                    strcpy(res, "HTTP/1.1 404 Not Found\r\n\r\n");
                }
            } else {
                // /api/?
                strcpy(res, "HTTP/1.1 404 Not Found\r\n\r\n");
            }
        } else {
            // /?
            strcpy(res, "HTTP/1.1 404 Not Found\r\n\r\n"); return;
      }
    } else {
        printf("Mehtod \"%s\" is unrecognizable\n", method);


    }

    // char file_path[64], buffer[1024];
    // if (strlen(request_route) == 1 &&
    //         compare_str(request_route, 0, "/", 0, 1) == 0) { // root
    //     sprintf(file_path, "%s/index.html", HTML_FOLDER);
    // } else if (compare_str(request_route, 0, "/login", 0, 6) == 0) { // login
    //     char id[16], passwd[16];
    //     get_from_two_str(client_header, "id=", "&", id);
    //     get_from_two_str(client_header, "passwd=", "", passwd);
    //     if (DBG) printf("In login, id=%s, passwd=%s\n", id, passwd);
    //     login(id, passwd, res);
    //     return;
    // } else if (compare_str(request_route, 0, "/logout", 0, 7) == 0) { // logout
    //     logout(res);
    //     return;
    // } else {
    //     sprintf(file_path, "%s%s", HTML_FOLDER, request_route);
    // }
    //
    // // load file
    // if (load_file_to_buffer(file_path, buffer) == -1) { // err
    //   strcpy(res, "HTTP/1.1 404 Not Found\r\n\r\n");
    // } else { // get file successfully
    //   strcpy(res, "HTTP/1.1 200 OK\r\n");
    //   strcat(res, buffer);
    //   strcat(res, "\r\n");
    // }
}
