CC=gcc
CFLAGS=-Wall

all: read_server write_server

read_server: read_server.o
	$(CC) -o $@ $^ ${CFLAGS}

read_server.o: server.c
	$(CC) -D READ_SERVER -c -o $@ $< ${CFLAGS}

write_server: write_server.o
	$(CC) -o $@ $^ ${CFLAGS}

write_server.o: server.c
	$(CC) -D WRITE_SERVER -c -o $@ $< ${CFLAGS}

clean:
	rm read_server write_server *.o