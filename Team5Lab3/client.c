#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>         // string manipulation
#include <sys/socket.h>
#include <unistd.h>         // read, write

#define PORT 5678
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
    strcpy(client_msg, "Client say hello");
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

    return 0;
}
