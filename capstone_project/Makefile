CC = gcc
CFLAGS = -Wall -Wextra -std=c11

COMMON = event.o eventstore.o storage.o

all: calendar server client

calendar: main.o $(COMMON)
	$(CC) $(CFLAGS) -o calendar main.o $(COMMON)

server: server.o $(COMMON)
	$(CC) $(CFLAGS) -o server server.o $(COMMON)

client: client.o
	$(CC) $(CFLAGS) -o client client.o

main.o: main.c eventstore.h storage.h
	$(CC) $(CFLAGS) -c main.c

event.o: event.c event.h
	$(CC) $(CFLAGS) -c event.c

eventstore.o: eventstore.c eventstore.h event.h
	$(CC) $(CFLAGS) -c eventstore.c

storage.o: storage.c storage.h eventstore.h
	$(CC) $(CFLAGS) -c storage.c

server.o: server.c eventstore.h storage.h
	$(CC) $(CFLAGS) -c server.c

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

clean:
	rm -f *.o calendar server client
