CC = gcc
CFLAGS = -Wall

all: server

server: server.c strfunc.c
	${CC} server.c strfunc.c ${CFLAGS} -o $@
clean:
	@rm -f server *.o
