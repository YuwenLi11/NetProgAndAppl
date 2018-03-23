#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>

int main(int argc, char *argv[])
{	if (argc == 2) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
    //sent the request to the assigned host
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //initial the address
    serv_addr.sin_family = AF_INET;  //use IPv4
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //use local host
    serv_addr.sin_port = htons(2222);  //port is 2222

    //send the name of the file to the server
    char fname[50];
    strcpy(fname,argv[1]);
    int bytesReceived = 0;
    char recvBuff[2048] = {0};
    printf("requested file name: %s\n", fname);
    sendto(sock, fname, sizeof(fname), 0,
        (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    bytesReceived = recvfrom(sock, recvBuff, sizeof(recvBuff) - 1, 0, NULL, 0);
    printf("bytesReceived = %d\n", bytesReceived);

    // no file matched
    if (bytesReceived == 0) {
        printf("There is no file in server named: %s\n", fname);
    } else if (bytesReceived < 0) {
        printf("\n Read Error \n");
    } else {
    		//create a file for receiving
        	FILE *fp;
        	printf("Receiving file...\n");
        	fp = fopen(fname, "ab");
           		if(NULL == fp) {
           			printf("Error opening file");
           			return 1;
           	}

           	printf("writing\n");
           	fflush(stdout);
           	fwrite(recvBuff, 1, bytesReceived, fp);

            fclose(fp);
            //check the file
           	printf("File received completely!");
    	}
	} else {
        	printf("Please enter the file name\n");
	}
    return 0;
}
