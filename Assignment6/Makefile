CC = gcc
CFLAGS = -g -Wall -pedantic
LD = gcc
LDFLAGS =
all: mush clean
mush: Functions.o mush.o
	$(LD) $(LDFLAGS) -o mush Functions.o mush.o
Functions.o: Functions.c
	$(CC) $(CFLAGS) -c -o Functions.o Functions.c
mush.o: mush.c
	$(CC) $(CFLAGS) -c -o mush.o mush.c
clean:
	rm *.o
