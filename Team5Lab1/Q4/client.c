#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main(int argc, char *argv[]){
	//the command line is appropriate
	if (argc == 2) {
		char *buffer = argv[1];
    	int len = strlen(buffer);
    	//check if the input is too big
    	if(len > 7) {
    		printf("The number is too large\n");
    	}
    	else {
    		//create a socket
    		int sock = socket(AF_INET, SOCK_STREAM, 0);
    	    //sent the request to the assigned host
    	    struct sockaddr_in serv_addr;
    	    memset(&serv_addr, 0, sizeof(serv_addr));  //initial the address
    	    serv_addr.sin_family = AF_INET;  //use IPv4
    	    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //use local host
    	    serv_addr.sin_port = htons(2222);  //port is 2222
    	    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

            write(sock, buffer, sizeof(buffer));
            //read the data from the server
            char buffer1[1000000];
            read(sock, buffer1, sizeof(buffer1)-1);
            printf("Your input is: %s\n", buffer);
            printf("Message form server: %s", buffer1);
            printf("\n");

            //close the socket
            close(sock);
    	}
    }
	// the command line has no input
	else {
    	printf("Please enter one number\n");
    }
    return 0;
}
