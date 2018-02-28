CC = g++
CFLAGS = -std=c++14
TARGET = udp_client

.SUFFIX: .cpp

all: ${TARGET}

${TARGET}: udp_client.cpp
	${CC} udp_client.cpp ${CFLAGS} -o $@

clean:
	@echo "rm *.o ${TARGET}"
	@rm *.o ${TARGET} 2>/dev/null || true # suppress error messages
