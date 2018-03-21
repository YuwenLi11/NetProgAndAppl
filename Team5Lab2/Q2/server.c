#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>         // string manipulation
#include <sys/socket.h>
#include <unistd.h>         // read, write

#define PORT 5678
#define BACKLOG 3 // queue length
#define MSG_SIZE 512
#define MAX_FILE_SIZE 2048
#define MAX_FILE_NAME_SIZE 128

int main() {

    // File preparation
    char file_name[MAX_FILE_NAME_SIZE] = {0};
    int file_size = 0;
    strcpy(file_name, "file");
    FILE *fp = fopen(file_name, "rb"); // read as binary file
    if (fp == NULL) {
        printf("Couldn't find file \"%s\" in the current folder\n", file_name);
        return -1;
    }
    // Get file size
    fseek(fp, 0, SEEK_END); // seek to the end
    file_size = ftell(fp); // get file_size
    fseek(fp, 0, SEEK_SET); // seek back to the beginning
    printf("File \"%s\" is read, size = %d bytes\n", file_name, file_size);

    char file_buffer[MAX_FILE_SIZE] = {0};

    if (fread(file_buffer, 1, file_size, fp) != file_size) {
        printf("Copy file to buffer failed\n");
        return -1;
    }
    printf("File \"%s\" is ready to be sent\n", file_name);


    // Create socket
    int sd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM for TCP
    if (sd < 1) {
        printf("Couldn't create socket\n");
        return -1;
    }
    printf("TCP socket created\n");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; // enable any ip_addr
    server.sin_port = htons(PORT);

    // Bind
    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Bind failed\n");
        return -1;
    }
    printf("Bind successful, listen to port %d\n", PORT);

    // Listen
    if (listen(sd, BACKLOG) < 0) {
        printf("Listen failed\n");
        return -1;
    }
    printf("Listen successful");

    // Accept
    // Define client variables
    struct sockaddr_in client;
    socklen_t client_len = sizeof(struct sockaddr_in);
    char client_msg[MAX_FILE_SIZE] = {0};
    int client_sd = accept(sd, (struct sockaddr*)&client, &client_len);
    if (client_sd < 0) {
        printf("Accept failed\n");
        return -1;
    }
    printf("Connection accepted, client socket is \"%d\"\n", client_sd);

    // Read
    int read_size = read(client_sd, client_msg, MSG_SIZE);
    if (read_size < 0) {
        printf("Read failed\n");
        return -1;
    }
    printf("Read from client:\n%s\n", client_msg);

    // Write
    if (write(client_sd, file_buffer, file_size) < 0) {
        printf("Write failed\n");
        return -1;
    }
    printf("Write successful\n");
    fclose(fp);

    return 0;
}
