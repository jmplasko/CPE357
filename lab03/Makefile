CC      = gcc
CFLAGS  = -Wall
OBJFILES= htable.c
TARGET  = htable

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -g -o $(TARGET) $(OBJFILES)

clean:
	$(RM) htable
