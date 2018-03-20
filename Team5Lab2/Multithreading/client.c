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
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
    //sent the request to the assigned host
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //initial the address
    serv_addr.sin_family = AF_INET;  //use IPv4
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //use local host
    serv_addr.sin_port = htons(2222);  //port is 2222
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    char fname[50];
    strcpy(fname,argv[1]);
    int bytesReceived = 0;
    char recvBuff[1000000];
    printf("file name: %s\n", fname);
    write(sock, fname, 50);

   	 /* Create file where data will be stored */
    FILE *fp;

	printf("Receiving file...");

	fp = fopen(fname, "ab");
    	if(NULL == fp)
    	{
       	 printf("Error opening file");
         return 1;
    	}


    /* Receive data in chunks of 256 bytes */
    while((bytesReceived = read(sock, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
	fflush(stdout);
        // recvBuff[n] = 0;
        fwrite(recvBuff, 1,bytesReceived,fp);
        // printf("%s \n", recvBuff);
    }

    if(bytesReceived < 0)
    {
        printf("\n Read Error \n");
    }

    printf("\nFile OK....Completed\n");
    return 0;
}
