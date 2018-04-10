CC = gcc
CFLAGS = -Wall

all: server
server: server.c strfunc.o
teststr: teststr.c strfunc.o
strfunc.o: strfunc.c
clean:
	@rm -f server teststr *.o
