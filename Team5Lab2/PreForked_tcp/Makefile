CC = gcc
CFLAGS = -Wall

all: client server
	mkdir -p client_folder server_folder
	mv client client_folder
	mv server server_folder
	cp 1800bytes.txt 900bytes.txt server_folder

client: client.c
	${CC} $< ${CFLAGS} -o $@
server: server.c
	${CC} $< ${CFLAGS} -o $@
clean:
	@rm -rf client_folder server_folder
