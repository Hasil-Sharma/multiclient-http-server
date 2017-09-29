#Makefile
CC = gcc
INCLUDE = /usr/lib
LIBS =
OBJS = 
CFLAGS = -g
all: run_server

run_server:
	$(CC) -o run_server headers/*.c  server/server.c $(CFLAGS) $(LIBS)

clean:
	rm -r run_server
