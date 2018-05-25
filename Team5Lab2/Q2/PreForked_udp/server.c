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
#include <signal.h>

int main(int argc, char *argv[]) {

    int num = 0;

    if (argc == 2)  {
        //get the number of pre-allocated children
        char proc[50];
        strcpy(proc,argv[1]);
        num = atoi(proc);
    } else {
        printf("Please enter one number\n");
        return -1;
    }

    struct sockaddr_in serv_addr;
    int listenfd = 0,ret;

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

    struct sockaddr_in clnt_addr;
    socklen_t clen=sizeof(clnt_addr);
    char fname[256];
    printf("Waiting...\n");

    int pid;
    for (int i = 0; i < num; i++) {
        pid = fork();

        if (pid == 0) { // child
            while (1) {
                printf("child %d is running\n", getpid());

                recvfrom(listenfd, fname, 256, 0, (struct sockaddr *)&clnt_addr, &clen);
                FILE *fp = fopen(fname,"rb");
                if (fp==NULL) {
                    printf("There is no file named: %s\n", fname);
                    char zero_buff[1] = {0};
                    sendto(listenfd, zero_buff, 0, 0,
                               (struct sockaddr *)&clnt_addr, clen);
                } else {
                   //Read data from file and send it
                     unsigned char buff[2048]={0};
                     int nread = fread(buff,1,2048,fp);

                     sendto(listenfd, buff, nread, 0,
                                (struct sockaddr *)&clnt_addr, clen);
                     printf("File sent!\n");
                }
            }
        }
    }

    // for killing zombies
    while (1) {
        int status;
        pid_t wait_pid = waitpid(-1, &status, 0);
        if (wait_pid <= 0) {
            break;
        }
        printf("child_pid: %d dead\n", wait_pid);
    }

    return 0;
}
