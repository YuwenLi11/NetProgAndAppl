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
     printf("Connected to client: %s:%d\n",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));

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

void *acceptHandleClient(void *sock)
{
    struct sockaddr_in fsin; // the address of a client
    unsigned int alen;       // length of client's address
    int listenfd = *(int *)sock;
    int connfd = accept(listenfd, (struct sockaddr *)&fsin, &alen);
    printf("Prethreaded child connection accepted.\n");
    int *new_sock = malloc(1);
    *new_sock = connfd;
    SendFileToClient(new_sock);
    return NULL;
}

int main(int argc, char *argv[]) {
	if (argc == 2)  {
	    //get the number of pre-allocated children
		char proc[50];
	    strcpy(proc,argv[1]);
	    int num = atoi(proc);

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

		// prethreading process
		for (int i = 0; i < num; ++i)
		{
			pthread_t child;
			int *new_sock = &listenfd;
			if (pthread_create(&child, NULL, acceptHandleClient, (void *)new_sock) < 0)
				printf("Could not creat thread");
			else
				printf("Prethreaded child %d\n", i + 1);

			// detach the child, parent don't need to wait the child finishing.
			if (pthread_detach(child) != 0)
				printf("Could not detach thread: %s\n");
		}

		//listen to the port, create one thread per request
		while(1)
		{
			clen=sizeof(c_addr);
			printf("Waiting...\n");
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
	} else {
		printf("Please enter one number\n");
	}
    return 0;
}
