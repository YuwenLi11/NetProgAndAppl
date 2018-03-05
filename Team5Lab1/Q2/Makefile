CC = gcc
CFLAGS = -Wall
TARGET = udp_client

.SUFFIXS: .c

udp_client: udp_client.c
	${CC} $< ${CFLAGS} -o $@
clean:
	@rm -rf ${TARGET}
