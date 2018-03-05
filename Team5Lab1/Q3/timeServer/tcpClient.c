#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int sockf = 0, n = 0;
    char buff[1024];
    struct sockaddr_in serv_addr;

    if(argc != 2)
    {
        printf("\n Usage is like: %s <ip of server> \n",argv[0]);
        return 1;
    }

    memset(buff, '0',sizeof(buff));
    if((sockf = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Socket could not be created \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n Error occured: in inet_pton \n");
        return 1;
    }

    if( connect(sockf, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error: Connection Failed \n");
       return 1;
    }

    while ( (n = read(sockf, buff, sizeof(buff)-1)) > 0)
    {
        buff[n] = 0;
        if(fputs(buff, stdout) == EOF)
        {
            printf("\n Error : in Fputs \n");
        }
    }

    if(n < 0)
    {
        printf("\n Error: in reading \n");
    }

    return 0;
}
