#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>         // string manipulation
#include <sys/socket.h>
#include <unistd.h>         // read, write

#define BACKLOG 10 // queue length
#define MAX_HEADER_SIZE 1024
#define MAX_DATA_SIZE 2048

struct client_param {
    int client_sd;
    char client_ip[32];
};

int start_socket(int port);
void *conn_handler(void *param);
int get_str_until_space(char *src, int src_start, char *dst);
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
        return (void *) -1;
    }
    printf("Client header ================\n");
    printf("%s", client_header);
    printf("==============================\n");

    // find out HTTP method and request file
    char method[8], file_name[32];
    int pos = get_str_until_space(client_header, 0, method); // find method
    pos = get_str_until_space(client_header, pos + 2, file_name); // find file_name
    printf("Method = %s, File_name = %s\n", method, file_name);

    // Respond client according given method and file name
    char res[MAX_HEADER_SIZE] = {0}, buffer[MAX_DATA_SIZE] = {0};
    if (strcmp(method, "GET") == 0) { // send file
        int file_size = load_file_to_buffer(file_name, buffer);
        if (file_size == -1) { // couldn't find file or other errors
            // 404 Not Found
            strcpy(res, "HTTP/1.1 404 Not Found\r\n\r\n");
            if (write(client_sd, res, strlen(res)) < 0) {
                printf("Write failed\n");
                return (void *) -1;
            }

            // Transfer Error Page
            if (strcmp(file_name, "") != 0) {
                sprintf(res, "<!doctype html><html><body>\
                    <h1>404 Not Found, File %s might not exist</h1>\
                    </body></html>\r\n\r\n", file_name);
            } else {
                sprintf(res, "<!doctype html><html><body>\
                    <h1>404 Not Found, Please request by given file name</h1>\
                    </body></html>\r\n\r\n");
            }
            if (write(client_sd, res, strlen(res)) < 0) {
                printf("Write failed\n");
                return (void *) -1;
            }
        } else {
            // 200 OK
            strcpy(res, "HTTP/1.1 200 OK\r\n\r\n");
            if (write(client_sd, res, strlen(res)) < 0) {
                printf("Write failed\n");
                return (void *) -1;
            }

            // Transfer data
            strcpy(res, "The Second Write\r\n\r\n");
            if (write(client_sd, buffer, file_size) < 0) {
                printf("Write failed\n");
                return (void *) -1;
            }
        }
    } else if (strcmp(method, "POST") == 0 || strcmp(method, "PUT") == 0) {
        // 403 Forbidden
        strcpy(res, "HTTP/1.1 403 Forbidden\r\n\r\n");
        if (write(client_sd, res, strlen(res)) < 0) {
            printf("Write failed\n");
            return (void *) -1;
        }

        // Transfer Error Page
        sprintf(res, "<!doctype html><html><body>\
            <h1>403 Forbidden</h1>\
            </body></html>\r\n\r\n");
        if (write(client_sd, res, strlen(res)) < 0) {
            printf("Write failed\n");
            return (void *) -1;
        }
    } else {
        // 400 Bad Request
        strcpy(res, "HTTP/1.1 400 Bad Request\r\n\r\n");
        if (write(client_sd, res, strlen(res)) < 0) {
            printf("Write failed\n");
            return (void *) -1;
        }

        // Transfer Error Page
        sprintf(res, "<!doctype html><html><body>\
            <h1>400 Bad Request</h1>\
            </body></html>\r\n\r\n");
        if (write(client_sd, res, strlen(res)) < 0) {
            printf("Write failed\n");
            return (void *) -1;
        }
    }

    printf("Write Response to %s\n\n", client_ip);
    close(client_sd);
    return (void *) 0;
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

    fread(buffer, MAX_DATA_SIZE, 1, file);
    fseek(file, 0L, SEEK_END); // in order to find file size
    int file_size = ftell(file);

    printf("Server read %s, size %d bytes\n", file_name, file_size);

    fclose(file);
    return file_size;
}
