include config.mk

SRC = client.c server.c
OBJ = $(SRC:.c=.o)

CC ?= gcc
CFLAGS ?= -Wall





%.o: %.c
	@$(CC) -c $(CFLAGS) $<

client: client.o
	@$(CC) -o $@ $^

server: server.o 
	@$(CC) -o $@ $^

run: server
	@echo "Running server..."

	@./server

clean:
	rm -f client server $(OBJ)

.PHONY: all clean run
