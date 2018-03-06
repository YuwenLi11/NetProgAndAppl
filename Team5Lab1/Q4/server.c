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
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //assign the address and port
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //initial server address
    serv_addr.sin_family = AF_INET;  //use IPV4
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //use local host
    serv_addr.sin_port = htons(2222);  //port is 2222
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    //listen the port
    listen(serv_sock, 20);
    //accept the request from client
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

    //read data from client
    char buffer[1000000];
    read(clnt_sock, buffer, sizeof(buffer)-1);
    int num = 1;
    char sym = buffer[0];
    int len = strlen(buffer);
    int valid = 0; //flag

    //check if the number is negative
    if (sym == '-') {
		valid = 2;
	}
    //check if the input is a number
    if (valid != 2) {
        for (int i = 0; i < len; i++) {
        	//check if the number is decimal
        	if(buffer[i] == '.') {
        		continue;
        	}

        	if(!(isdigit(buffer[i]))) {
        		valid = 1;
        		break;
        	}
        }
    }
    //check if the number is zero
    if (valid != 1 && valid != 2) {
       num = atoi(buffer);
       if (num == 0) {
        valid = 3;
        }
    }


    switch (valid) {
		//case 0 the input is appropriate
    	case 0: {
		int result1 = sqrt(num);
		char result[1000000];
		sprintf(result, "%d", result1);
		write(clnt_sock, result, sizeof(result));
		break;
	}
    	//case 1 the input is not a number
	case 1: {
		char str2[] = "please input a number";
		write(clnt_sock, str2, sizeof(str2));
		break;
	}
	//case 2,3 the input is not bigger than zero
	case 2:
	case 3:	{
		char str3[] = "please input a number bigger than zero";
		write(clnt_sock, str3, sizeof(str3));
		break;
	}
    }

    //close socket
    memset(buffer, 0, sizeof(buffer));
    close(clnt_sock);
    close(serv_sock);
    return 0;
}
