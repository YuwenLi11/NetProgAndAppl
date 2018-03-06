CC = gcc
CFLAGS = -Wall
TARGET_1 = udp_client
RESOURCE_1 = udp_client.c
TARGET_2 = udp_server
RESOURCE_2 = udp_server.c

all: ${TARGET_1} ${TARGET_2}

${TARGET_1}: ${RESOURCE_1}
	${CC} $< ${CFLAGS} -o $@
${TARGET_2}: ${RESOURCE_2}
	${CC} $< ${CFLAGS} -o $@
clean:
	@rm -rf ${TARGET_1} ${TARGET_2}
