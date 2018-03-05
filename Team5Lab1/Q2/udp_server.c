#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define UDP_DAYTIME_PORT 5678 // Default should be 12, but reserved by OS

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
    server.sin_addr.s_addr = INADDR_ANY; // enable any ip_addr
    server.sin_port = htons(UDP_DAYTIME_PORT);

    // Bind
    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Bind failed\n");
        return 1;
    }
    printf("Bind successful, listen to port %d\n", UDP_DAYTIME_PORT);

    // Receive message
    char msg[100];
    if (recvfrom(sd, msg, sizeof(msg) - 1, 0, NULL, 0) < 0) {
        printf("Recvfrom failed\n");
        return 1;
    }
    printf("Recvfrom successful, message from client is \"%s\"\n", msg);


    return 0;
}
