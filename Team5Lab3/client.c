#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>         // string manipulation
#include <sys/socket.h>
#include <unistd.h>         // read, write

#define PORT 5678
#define MSG_SIZE 2048

int main() {
    // User input file_name
    char file_name[128];
    printf("Please input file name to request: ");
    scanf("%s", file_name);

    // Create socket
    int sd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM for TCP
    if (sd < 1) {
        printf("Couldn't create socket\n");
        return -1;
    }
    printf("TCP socket created\n");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);

    // Connect
    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connect failed\n");
        return -1;
    }
    printf("Connect successful\n");

    // Write
    // Define message to be written
    char client_msg[MSG_SIZE] = {0};
    sprintf(client_msg, "GET /%s HTTP/1.1\r\n\r\n", file_name);
    if (write(sd, client_msg, strlen(client_msg)) < 0) {
        printf("Write failed\n");
        return -1;
    }
    printf("Write successful\n");

    // Read
    // Define message buffer from server
    char server_msg[MSG_SIZE] = {0};
    int read_size = read(sd, server_msg, MSG_SIZE);
    if (read_size < 0) {
        printf("Read failed\n");
        return -1;
    }

    printf("Read from server:\n%s\n", server_msg);
    char file_from_server[MSG_SIZE] = {0};
    read_size = read(sd, file_from_server, MSG_SIZE);
    if (read_size < 0) {
        printf("Read failed\n");
        return -1;
    }
    if (strcmp(server_msg, "HTTP/1.1 200 OK\r\n\r\n") == 0) {
        // write file
        FILE *file_to_write;
        file_to_write = fopen(file_name, "wb");
        fwrite(file_from_server, read_size, 1, file_to_write);
        fclose(file_to_write);
    } else {
        printf("Fail message from server:\n%s\n", file_from_server);
    }

    return 0;
}
