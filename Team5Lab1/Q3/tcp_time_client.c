#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
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

// save cur_time as a string into time_str
void time_to_string(char *time_str, time_t cur_time) {
    strcpy(time_str, ctime(&cur_time));
    time_str[strlen(time_str) - 1] = '\0'; // remove \n
}

// request time to specific time server and get time_t as return value
time_t request_time(char *time_server_ip) {
    // Create socket
    int sd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM for TCP
    if (sd < 1) {
        printf("Couldn't create socket\n");
        exit(1);
    }
    printf("TCP socket created\n");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(time_server_ip);
    server.sin_port = htons(TCP_TIME_PORT);

    // Connect to server
    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connect failed\n");
        exit(1);
    }
    printf("Connection established\n");

    // Send message to server
    unsigned int time_buf = 0;
    if (send(sd, &time_buf, sizeof(time_buf), 0) < 0) {
        printf("Send failed\n");
        exit(1);
    }

    // Receive message from server
    if (recv(sd, &time_buf, sizeof(time_buf), 0) < 0) {
        printf("Recv failed\n");
        exit(1);
    }

    // transfer time format
    time_t trans_time = trans_time_format(time_buf);

    close(sd);

    return trans_time;
}

int main(int argc, char *argv[]) {
    // Check input should contain two ip address
    if (argc != 3) {
        printf("Two time server IP Addresses are required\n");
        return 1;
    }

    // Send requests to two TIME servers
    printf("Send time request to %s...\n", argv[1]);
    time_t time1 = request_time(argv[1]);
    printf("Send time request to %s...\n", argv[2]);
    time_t time2 = request_time(argv[2]);

    // Transfer time_t to string
    char time_str1[100], time_str2[100];
    time_to_string(time_str1, time1);
    time_to_string(time_str2, time2);
    printf("Receive from server %s: %s\n", argv[1], time_str1);
    printf("Receive from server %s: %s\n", argv[2], time_str2);

    // Calculate time difference
    time_t diff = time2 > time1 ? time2 - time1 : time1 - time2;
    printf("The difference between two servers are %d seconds\n", (int)diff);

    return 0;
}
