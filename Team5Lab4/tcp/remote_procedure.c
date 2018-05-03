#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

static off_t file_size(const char* fname);

char* content = NULL;

char** get_file_3_svc(const char* fname){

    if(content != NULL){
        free(content);
        content = NULL;
    }

    // open file
    int fd = open(fname,O_RDWR);
    if (fd < 0) {
   	 printf("There is no file named: %s\n", fname);
   	 return &content;
    }

    printf("Reading the file: %s\n", fname);
    off_t fsz = file_size(fname);
    content = (char*)malloc(fsz);
    memset(content, 0, sizeof(fsz));
    
    int nread = read(fd,content,fsz);

    if(nread < 0){
        printf("read failed");
        free(content);
        exit(-1);
    }
    printf("file tranfered\n");

    content[nread] = '\0';
    // return a pointer to the space
    return &content;
  }

static off_t file_size(const char* fname){
    struct stat st;

    if(stat(fname, &st) == 0)
        return st.st_size;

    return -1;
}


