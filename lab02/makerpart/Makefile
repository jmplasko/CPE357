CC	= gcc
CFLAGS	= -Wall
OBJFILES= hellofunc.c hellomake.c
TARGET	= hellomake

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) -I.

prog: hellomake
	$(CC) $(CFLAGS) -o hellomake $(OBJFILES) -I.

test: $(TARGET)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) -I.
	./hellomake <a> output.txt

clean:
	$(RM) hellofunc.o hellomake.o hellomake
