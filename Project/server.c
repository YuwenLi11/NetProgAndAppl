#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>         // string manipulation
#include <sys/socket.h>
#include <unistd.h>         // read, write

#include "strfunc.h"

#define BACKLOG 10 // queue length
#define MAX_HEADER_SIZE 1024
#define MAX_FILE_SIZE 2048
#define HTML_FOLDER "html"

struct client_param {
    int client_sd;
    char client_ip[32];
};

int start_socket(int port);
void *conn_handler(void *param);
void get_response(char *res, char *client_header);
int load_file_to_buffer(char *file_name, char *buffer);

int main() {
    int sd = start_socket(5678);

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

    // Listen
    if (listen(sd, BACKLOG) < 0) {
        printf("Listen failed\n");
        exit(-1);
    }
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
    struct client_param *cp = (struct client_param *)param;
    int client_sd = cp->client_sd;
    char* client_ip = cp->client_ip;

    // Read
    char client_header[MAX_HEADER_SIZE] = {0};
    int read_size = read(client_sd, client_header, MAX_HEADER_SIZE);
    if (read_size < 0) {
        printf("Read failed\n");
        exit(-1);
    }
    printf("Client header ================\n");
    printf("%s", client_header);
    printf("==============================\n");

    // Write
    // Define response
    char res[MAX_HEADER_SIZE] = {0};
    get_response(res, client_header);
    if (write(client_sd, res, strlen(res)) < 0) {
        printf("Write failed\n");
        exit(-1);
    }
    printf("Write Response to %s\n\n", client_ip);
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
    for (int j = 0; j < line_cnt; j++) {
        printf("Each line: %s\n", line[j]);
    }

    // Determine method
    int cursor = 0;
    char method[8], request_route[32];
    cursor = get_str_until_space(line[0], cursor, method);
    printf("Method = %s\n", method);
    cursor = get_str_until_space(line[0], cursor, request_route);
    printf("Request Route = %s\n", request_route);

    // Determine response file path
    char file_path[64], buffer[1024];
    if (strlen(request_route) == 1 &&
            compare_str(request_route, 0, "/", 0, 1) == 0) { // root
        sprintf(file_path, "%s/index.html", HTML_FOLDER);
    } else {
        sprintf(file_path, "%s%s", HTML_FOLDER, request_route);
    }

    if (load_file_to_buffer(file_path, buffer) == -1) { // err
      strcpy(res, "HTTP/1.1 404 Not Found\r\n\r\n");
    } else { // get file successfully
      strcpy(res, "HTTP/1.1 200 OK\r\n\r\n");
      strcat(res, buffer);
      strcat(res, "\r\n");
    }
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
