#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>         // string manipulation
#include <sys/socket.h>
#include <unistd.h>         // read, write

#define BACKLOG 10 // queue length
#define MAX_HEADER_SIZE 1024

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
    printf("TCP socket created\n");

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
    printf("Bind successful, listen to port %d\n", port);

    return sd;
}

/************************************************************
 * Function: compare_str
 *   Compare two string with different start position by given length
 *   EX - compare("abc", 1 "bcd", 0, 2) return 0, since "bc" equals to "bc"
 * Parameters:
 *   str1, str2 - two strings
 *   start1, start2 - start position of the string
 *   len - the length to be compare from two start position
 * Returns:
 *   0 - two string are the same, with the given parameters
 *   1 - two string are not the same, with the given parameters
 ************************************************************/
int compare_str(char *str1, int start1, char *str2, int start2, int len) {
    if (len > strlen(str1) || len > strlen(str2)) return 1; // corner case

    int cnt = 0;
    while (start1 < strlen(str1) && start2 < strlen(str2) && cnt < len) {
        if (str1[start1++] != str2[start2++]) {
            return 1; // not the same
        }
        cnt++;
    }
    return 0; // the same
}

void get_request_url(int client_sd, char* client_header, char* client_ip) {
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
    } else if (compare_str(client_header, 0, "POST", 0, 4) == 0) {
        printf("POST Request\n");
    }
}

int main() {
    int sd = start_socket(5678);

    // Listen
    if (listen(sd, BACKLOG) < 0) {
        printf("Listen failed\n");
        return -1;
    }
    printf("Listen successful\n");

    // Accept
    // Define client variables
    struct sockaddr_in client;
    socklen_t client_len = sizeof(struct sockaddr_in);
    char client_header[MAX_HEADER_SIZE] = {0};
    int client_sd = accept(sd, (struct sockaddr*)&client, &client_len);
    char client_ip[24] = {0};
    if (client_sd < 0) {
        printf("Accept failed\n");
        exit(-1);
    }
    // Format client ip address and port into client_ip
    sprintf(client_ip, "%s:%d", inet_ntoa(client.sin_addr), client.sin_port);
    printf("Connection accepted with client %s\n", client_ip);

    // Read
    get_request_url(client_sd, client_header, client_ip);
    // int read_size = read(client_sd, client_header, MAX_HEADER_SIZE);
    // if (read_size < 0) {
    //     printf("Read failed\n");
    //     return -1;
    // }
    // printf("Read from client %s\n", client_ip);
    // printf("========================================\n");
    // printf("%s", client_header);
    // printf("========================================\n");

    // Write
    // Define response
    char server_msg[MAX_HEADER_SIZE] = {0};
    strcpy(server_msg, "HTTP/1.1 200 OK\r\n\r\n");
    strcat(server_msg, "Hello world\r\n");
    if (write(client_sd, server_msg, strlen(server_msg)) < 0) {
        printf("Write failed\n");
        return -1;
    }
    printf("Write successful\n");

    close(sd);

    return 0;
}
