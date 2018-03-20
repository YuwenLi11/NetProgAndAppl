CC = gcc
CFLAGS = -Wall

all: client server
	mkdir client_folder server_folder
	mv client client_folder
	mv server server_folder

client: client.c
	${CC} $< ${CFLAGS} -o $@
server: server.c
	${CC} $< ${CFLAGS} -o $@
clean:
	@rm -rf client_folder server_folder
