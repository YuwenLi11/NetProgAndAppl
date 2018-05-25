CC = gcc
CFLAGS = -Wall
TARGET = tcp_time_client
RESOURCE = tcp_time_client.c

all: ${TARGET}

${TARGET}: ${RESOURCE}
	${CC} $< ${CFLAGS} -o $@

clean:
	@rm -rf ${TARGET}
