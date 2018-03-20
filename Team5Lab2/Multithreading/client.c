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
	int sock = socket(AF_INET, SOCK_STREAM, 0);
    //sent the request to the assigned host
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //initial the address
    serv_addr.sin_family = AF_INET;  //use IPv4
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //use local host
    serv_addr.sin_port = htons(2222);  //port is 2222
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    //send the name of the file to the server
    char fname[50];
    strcpy(fname,argv[1]);
    int bytesReceived = 0;
    char recvBuff[1024];
    printf("requested file name: %s\n", fname);
    write(sock, fname, 50);
    bytesReceived = read(sock, recvBuff, 1024);

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

           	// in case the file is less than 1 bytes
           	printf("writing\n");
           	fflush(stdout);
           	fwrite(recvBuff, 1,bytesReceived,fp);
           	//keep writing
           	while((bytesReceived = read(sock, recvBuff, 1024)) > 0) {
           		printf("writing\n");
           		fflush(stdout);
           		fwrite(recvBuff, 1,bytesReceived,fp);
           	}
            fclose(fp);
            //check the file
           	printf("File received completely!");
    	}
	} else {
        	printf("Please enter the file name\n");
	}
    return 0;
}
