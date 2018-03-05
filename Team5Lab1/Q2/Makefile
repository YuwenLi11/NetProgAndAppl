CC = gcc
CFLAGS = -Wall
TARGET = udp_client

${TARGET}: udp_client.c
	${CC} $< ${CFLAGS} -o $@
clean:
	@rm -rf ${TARGET}
