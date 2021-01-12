CC	= gcc
CFLAGS	= -Wall
OBJFILES= uniq.c
TARGET	= uniq

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -g -o $(TARGET) $(OBJFILES)

uniq:
	$(CC) $(CFLAGS) -g -o uniq $(OBJFILES)

test: $(TARGET)
	$(CC) $(CFLAGS) -g -o $(TARGET) $(OBJFILES)
	./$(TARGET) <new.txt

clean:
	$(RM) uniq
