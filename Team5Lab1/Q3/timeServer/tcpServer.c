#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <errno.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int listenf = 0, connf = 0;
    struct sockaddr_in serv_addr;

    char buff[1025];
    time_t ticks;

    listenf = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(buff, '0', sizeof(buff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenf, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenf, 10);

    while(1)
    {
        connf = accept(listenf, (struct sockaddr*)NULL, NULL);

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connf, buff, strlen(buff));

        close(connf);
        sleep(1);

}
