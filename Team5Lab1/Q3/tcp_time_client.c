#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define TCP_TIME_PORT 37
#define TRANSFER_OFFSET 2208988800U

// transfer raw_time (32 bits from server) to a time_t value
time_t trans_time_format(unsigned int raw_time) {
    return ntohl(raw_time) - TRANSFER_OFFSET;
}

void time_to_string(char *time_str, time_t cur_time) {
    strcpy(time_str, ctime(&cur_time));
    time_str[strlen(time_str) - 1] = '\0'; // remove \n
}

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
    time_t trans_time = trans_time_format(time_buf);
    char time_str[100];
    time_to_string(time_str, trans_time);

    printf("Receive from server: %s\n", time_str);

    return 0;
}
