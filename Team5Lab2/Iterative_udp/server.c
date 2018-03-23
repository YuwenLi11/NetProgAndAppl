#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>
#include <ctype.h>

int main(){
    //create a socket
    int serv_sock = socket(AF_INET, SOCK_DGRAM, 0);
    //assign the address and port
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //initial server address
    serv_addr.sin_family = AF_INET;  //use IPV4
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //use local host
    serv_addr.sin_port = htons(2222);  //port is 2222
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    char fname[50];

    //Iteratively handle each requet
    while(1) {
    	printf("Waiting...\n");
        //accept the request from client
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_size = sizeof(clnt_addr);
        recvfrom(serv_sock, fname, 256, 0,
            (struct sockaddr *)&clnt_addr, &clnt_addr_size);
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
                   sendto(serv_sock, buff, sizeof(buff), 0,
                    (struct sockaddr *)&clnt_addr, clnt_addr_size);
               }
               if (nread < 1024) {
                   if (feof(fp)) {
                   	printf("End of file\n");
                   	printf("File transfer completed\n");
                   }
                   if (ferror(fp)) {
                       printf("Error reading\n");
                   }
                   break;
               }
           }
          }

        sleep(2);
    }
}
