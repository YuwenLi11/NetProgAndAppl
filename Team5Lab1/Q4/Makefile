CC = gcc
CFLAGS = -Wall
TARGET_1 = client
TARGET_2 = server

all: client server

client: client.c
	${CC} $< ${CFLAGS} -o $@
server: server.c
	${CC} $< ${CFLAGS} -o $@
clean:
	@rm -rf client server