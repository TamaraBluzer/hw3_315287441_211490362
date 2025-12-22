CC = gcc
CFLAGS = -Wall -Wextra -g

all: hw3server hw3client

hw3server: hw3server.c chat.h
	$(CC) $(CFLAGS) -o hw3server hw3server.c

hw3client: hw3client.c chat.h
	$(CC) $(CFLAGS) -o hw3client hw3client.c

clean:
	rm -f hw3server hw3client

.PHONY: all clean
