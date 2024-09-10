include config.mk

SRC = client.c server.c
OBJ = ${SRC:.c=.o}

all: client server

.c.o:
	${CC} -c ${CFLAGS} $<

client: client.o
	${CC} -o $@ $<

server: server.o
	${CC} -o $@ $<

clean:
	rm -f client server ${OBJ}

.PHONY: all clean
