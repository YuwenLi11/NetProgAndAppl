#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define UDP_DAYTIME_PORT 5678 // Default should be 13, but reserved by OS

int main(int argc, char *argv[]) {
    // Create socket
    int sd = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM for UDP
    if (sd < 1) {
        printf("Couldn't create socket\n");
        return 1;
    }
    printf("UDP socket created\n");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // only test for localhost
    server.sin_port = htons(UDP_DAYTIME_PORT);

    // Send message to server
    if (sendto(sd, "", 0, 0,
        (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Sento failed\n");
        return 1;
    }
    printf("Sent request to server\n");

    // Receive response from server
    char rsp[100];
    if (recvfrom(sd, rsp, sizeof(rsp) - 1, 0, NULL, 0) < 0) {
        printf("Recvfrom failed\n");
        return 1;
    }
    printf("Receive DAYTIME from server: \"%s\"\n", rsp);

    return 0;
}
