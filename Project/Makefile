CC = gcc
CFLAGS = -Wall

all: server
server: server.c request.o strfunc.o
	${CC} server.c request.o strfunc.o ${CFLAGS} -lpthread -o $@
request.o: request.c
teststr: teststr.c strfunc.o
strfunc.o: strfunc.c
clean:
	@rm -f server teststr *.o
