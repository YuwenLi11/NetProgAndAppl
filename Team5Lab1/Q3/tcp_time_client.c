#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define TCP_TIME_PORT 37
#define TRANSFER_OFFSET 2208988800U

int main(int argc, char *argv[]) {
    // Create socket
    int sd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM for TCP
    if (sd < 1) {
        printf("Couldn't create socket\n");
        return 1;
    }
    printf("TCP socket created\n");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("129.6.15.28"); // test for TIME server
    server.sin_port = htons(TCP_TIME_PORT);

    // Connect to server
    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connect failed\n");
        return 1;
    }
    printf("Connection established\n");

    // Send message to server
    unsigned int time_buf = 0;
    if (send(sd, &time_buf, sizeof(time_buf), 0) < 0) {
        printf("Send failed\n");
        return 1;
    }

    // Receive message from server
    if (recv(sd, &time_buf, sizeof(time_buf), 0) < 0) {
        printf("Recv failed\n");
        return 1;
    }

    // transfer time format
    time_t trans_time = ntohl(time_buf) - TRANSFER_OFFSET;
    char time_str[100];
    strcpy(time_str, ctime(&trans_time));
    time_str[strlen(time_str) - 1] = '\0'; // remove \n

    printf("Receive from server: %s\n", time_str);

    return 0;
}
