#include <stdio.h>
#include <rpc/rpc.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "get_file.h"

static void write_to_file(const char* fname, char* buf);
unsigned int check_file_size(const char *fname);

int main(int argc, char *argv[]) {
    CLIENT *handle = NULL;
    char **file_content = NULL;

    char *server = "localhost";
    char *fname = argv[1];

    
    if (argc == 2) {
        /* create client handle */
        if ((handle = clnt_create(server, GETFILEPROG, GETFILEVERS, "udp")) == NULL) {
            /* couldn't establish connection with server */
        		printf("Can not access rpc service.\n");
            exit(1);
        }

        /* first call the remote procedure */
        file_content = get_file_3(fname, handle);
        if (file_content == NULL){
            printf("There is no file in server named: %s\n", fname);
            exit(1);
        }


        /* save file*/
        write_to_file(fname, *file_content);
        int size = check_file_size(fname);
        printf("The size of the file received: %d\n", size);
        clnt_destroy(handle); /* done with handle */
        return 0;
    } else {
    		printf("Please enter the file name\n");
        exit(1);
    }
}

static void write_to_file(const char* fname, char* buf){

    if(fname != NULL){
        // prepare to write
        int fd = open(fname, O_CREAT|O_RDWR, S_IRUSR|S_IRGRP);
        if(fd < 0){
            printf("error to open file: %s\n", fname);
            exit(-1);
        }

        write(fd, buf, (int)strlen(buf));
    }
}

unsigned int check_file_size(const char *fname)  
{  
    unsigned int filesize = -1;      
    struct stat statbuff;  
    if(stat(fname, &statbuff) < 0){  
        return filesize;  
    }else{  
        filesize = statbuff.st_size;  
    }  
    return filesize;  
}  
