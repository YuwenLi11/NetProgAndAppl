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

struct client_param {
    int client_sd;
    char client_ip[32];
};

int start_socket(int port);
void *conn_handler(void *param);
void get_response(char *res, char *client_header);

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
    strcpy(res, "HTTP/1.1 200 OK\r\n\r\n");
    char line[32][128];
    int i = 0, line_cnt = 0;
    while (i < strlen(client_header)) {
        i = get_str_line(client_header, i, line[line_cnt]);
        line_cnt++;
    }
    for (int j = 0; j < line_cnt; j++) {
        printf("Each line: %s\n", line[j]);
    }
}
