
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

int listenfd;
char fname[256];
struct sockaddr_in clnt_addr;
socklen_t clen;

void *SendFileToClient(void *arg) {
    FILE *fp = fopen(fname,"rb");
    if (fp == NULL) {
        printf("There is no file named: %s\n", fname);
        char zero_buff[1] = {0};
        sendto(listenfd, zero_buff, 0, 0,
           (struct sockaddr *)&clnt_addr, clen);
    } else {
        // Read data from file and send it
        unsigned char buff[2048]={0};
        int nread = fread(buff,1,2048,fp);
        sendto(listenfd, buff, nread, 0,
            (struct sockaddr *)&clnt_addr, clen);
        printf("File sent!\n");
    }

}

int main(int argc, char *argv[]) {
	if (argc == 2)  {
		char proc[50];
	    strcpy(proc,argv[1]);
	    int num = atoi(proc);

		struct sockaddr_in serv_addr;
		listenfd = 0;
		int ret;
		pthread_t tid;

		//server's information
		listenfd = socket(AF_INET, SOCK_DGRAM, 0);
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

		// prethreading process
		for (int i = 0; i < num; ++i)
		{
			pthread_t child;
			int *new_sock = malloc(1);
			*new_sock = listenfd;
			if (pthread_create(&child, NULL, SendFileToClient, (void *)new_sock) < 0)
				printf("Could not creat thread: %s\n");
			printf("Prethreaded child %d created.\n", i + 1);

			// detach the child, parent don't need to wait the child finishing.
			if (pthread_detach(child) != 0)
				printf("Could not detach thread: %s\n");
		}

		while(1)
		{
			clen = sizeof(clnt_addr);
			printf("Waiting...\n");
			recvfrom(listenfd, fname, 256, 0, (struct sockaddr *)&clnt_addr, &clen);

			int err = pthread_create(&tid, NULL, SendFileToClient, &ret);
			pthread_join(err, NULL);
		}
		return 0;
	} else {
		printf("Please enter one number\n");
	}
}
