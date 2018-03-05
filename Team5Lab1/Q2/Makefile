CC = gcc
CFLAGS = -Wall
TARGET_1 = udp_client
TARGET_2 = udp_server

all: udp_client udp_server

udp_client: udp_client.c
	${CC} $< ${CFLAGS} -o $@
udp_server: udp_server.c
	${CC} $< ${CFLAGS} -o $@
clean:
	@rm -rf udp_client udp_server
