CC = gcc

CFLAGS = -g -Wall -std=c99

TARGET=smallsh

all: $(TARGET)

$(TARGET): smallsh.o Command.o DynamicArray.o
	$(CC) $(CFLAGS) -o smallsh smallsh.o Command.o DynamicArray.o

smallsh.o: smallsh.c Command.h DynamicArray.h
	$(CC) $(CFLAGS) -c smallsh.c

Command.o: Command.c Command.h
	$(CC) $(CFLAGS) -c Command.c

DynamicArray.o: DynamicArray.c DynamicArray.h
	$(CC) $(CFLAGS) -c DynamicArray.c

clean:
	rm -f *.o $(TARGET) 
