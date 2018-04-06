#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>         // string manipulation
#include <sys/socket.h>
#include <unistd.h>         // read, write

#define PORT 5678
#define BACKLOG 3 // queue length
#define MSG_SIZE 1024

int main() {
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
    char client_msg[MSG_SIZE] = {0};
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
    // Define response
    char server_msg[MSG_SIZE] = {0};
    strcpy(server_msg, "Server say hello");
    if (write(client_sd, server_msg, strlen(server_msg)) < 0) {
        printf("Write failed\n");
        return -1;
    }
    printf("Write successful\n");

    return 0;
}
