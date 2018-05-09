CC = gcc
CFLAGS = -Wall

all: server
server: server.c jsmn.o request.o strfunc.o
	${CC} server.c jsmn.o request.o strfunc.o ${CFLAGS} -lpthread -o $@
request.o: request.c
teststr: teststr.c strfunc.o jsmn.o
strfunc.o: strfunc.c jsmn.o
jsmn.o: jsmn.c
clean:
	@rm -f server teststr *.o
