#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

struct sockaddr_in c_addr;
char fname[100];
// send file to client function
void *SendFileToClient(void *arg)
{
	 //information from the client
     int connfd = *(int*)arg;
     printf("Connection accepted and id: %d\n",connfd);
     printf("Connected to Clent: %s:%d\n",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));

     //file which requested by client
     read(connfd, fname,256);
     printf("Message form client: %s\n", fname);

     FILE *fp = fopen(fname,"rb");
     if (fp==NULL) {
    	 printf("There is no file named: %s\n", fname);
     } else {
        //Read data from file and send it
    	 while(1) {
    		 unsigned char buff[1024]={0};
    		 int nread = fread(buff,1,1024,fp);

        //If read was success, send data

            if(nread > 0) {
                write(connfd, buff, nread);
            }
            if (nread < 1024) {
                if (feof(fp)) {
                	printf("End of file\n");
                	printf("File transfer completed for id: %d\n",connfd);
                }
                if (ferror(fp)) {
                    printf("Error reading\n");
                }
                break;
            }
        }
       }
     //close the thread
     printf("Closing Connection for id: %d\n",connfd);
     close(connfd);
     shutdown(connfd,SHUT_WR);
     sleep(2);
}

int main() {
    int connfd = 0,err;
    pthread_t tid;
    struct sockaddr_in serv_addr;
    int listenfd = 0,ret;
    size_t clen=0;

    //server's information
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd<0)
	{
	  printf("Error in socket creation\n");
	  exit(2);
	}

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(2222);

    ret=bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));
    if(ret<0)
    {
      printf("Error in bind\n");
      exit(2);
    }

    if(listen(listenfd, 10) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }

    //listen to the port, create one thread per request

    while(1)
    {	printf("Waiting\n");
        clen=sizeof(c_addr);
        connfd = accept(listenfd, (struct sockaddr*)&c_addr,&clen);
        if(connfd<0)
        {
	  printf("Error in accept\n");
	  continue;
	}
        err = pthread_create(&tid, NULL, SendFileToClient, &connfd);
        if (err != 0)
         {
        	printf("\ncan't create thread :[%s]", strerror(err));
         }
        pthread_join(err,NULL);
   }
    close(connfd);
    return 0;
}
