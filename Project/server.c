#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>         // string manipulation
#include <sys/socket.h>
#include <unistd.h>         // read, write

#define BACKLOG 10 // queue length
#define MAX_HEADER_SIZE 1024
#define MAX_URL_SIZE 1024

#define UNKNOWN_REQUEST_ID 1000
#define GET_REQUEST_ID 1001
#define POST_REQUEST_ID 1002

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
 * Function: compare_str
 *   Compare two string with different start position by given length
 *   EX - compare_str("abc", 1, "bcd", 0, 2) return 0, since "bc" equals to "bc"
 * Parameters:
 *   str1, str2 - two strings
 *   start1, start2 - start position of the string
 *   len - the length to be compare from two start position
 * Returns:
 *   0 - two string are the same, with the given parameters
 *   1 - two string are not the same, with the given parameters
 ************************************************************/
int compare_str(char *str1, int start1, char *str2, int start2, int len) {
    // check corner case
    if (start1 + len > strlen(str1) || start2 + len > strlen(str2)) return 1;

    int cnt = 0;
    while (start1 < strlen(str1) && start2 < strlen(str2) && cnt < len) {
        if (str1[start1++] != str2[start2++]) {
            return 1; // not the same
        }
        cnt++;
    }
    return 0; // the same
}

/************************************************************
 * Function: get_str_until_space
 *   Get substr of src from given position until the first space and put in dst
 *   EX - get_str_until_space("abc def", 0, dst) return 3, dst will be "abc"
 * Parameters:
 *   src - original string
 *   src_start - the start position (including) of src
 *   dst - the buffer that the substring will be put into
 * Returns:
 *   the position of next space or the length of src (it traverses to the end)
 ************************************************************/
int get_str_until_space(char *src, int src_start, char *dst) {
    int i;
    for (i = 0; src_start < strlen(src) && src[src_start] != ' '; i++) {
        dst[i] = src[src_start];
        src_start++;
    }
    dst[i] = '\0';
    return src_start;
}

/************************************************************
 * Function: get_request_url
 *   Read from client and get url from the request
 * Parameters:
 *   src - original string
 *   src_start - the start position (including) of src
 *   dst - the buffer that the substring will be put into
 * Returns:
 *   the position of next space or the length of src (it traverses to the end)
 ************************************************************/
int get_request_url(int client_sd, char *client_ip, char *url) {
    char client_header[MAX_HEADER_SIZE] = {0};
    int read_size = read(client_sd, client_header, MAX_HEADER_SIZE);
    if (read_size < 0) {
        printf("Read failed\n");
        exit(-1);
    }
    printf("Read from client %s\n", client_ip);
    printf("========================================\n");
    printf("%s", client_header);
    printf("========================================\n");

    if (compare_str(client_header, 0, "GET", 0, 3) == 0) {
        printf("GET Request\n");
        get_str_until_space(client_header, 4, url);
        return GET_REQUEST_ID;
    } else if (compare_str(client_header, 0, "POST", 0, 4) == 0) {
        printf("POST Request\n");
        get_str_until_space(client_header, 5, url);
        return POST_REQUEST_ID;
    } else {
        return UNKNOWN_REQUEST_ID;
    }
}

struct client_param {
    int client_sd;
    char client_ip[32];
};

void* conn_handler(void *param) {
    struct client_param *cp = (struct client_param *)param;
    int client_sd = cp->client_sd;
    char* client_ip = cp->client_ip;

    // Read
    char url[MAX_URL_SIZE] = {0};
    int method = get_request_url(client_sd, client_ip, url);
    printf("Method = %d, url = %s\n", method, url);

    // Write
    // Define response
    char server_msg[MAX_HEADER_SIZE] = {0};
    strcpy(server_msg, "HTTP/1.1 200 OK\r\n\r\n");
    strcat(server_msg, "Hello world\r\n");
    if (write(client_sd, server_msg, strlen(server_msg)) < 0) {
        printf("Write failed\n");
        exit(-1);
    }
    printf("Write successful\n\n");
    close(client_sd);
    return 0;
}

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
