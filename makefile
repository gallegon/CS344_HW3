CC = gcc

CFLAGS = -g -Wall -std=c99

TARGET=smallsh

all: $(TARGET)

$(TARGET): smallsh.o Command.o DynamicArray.o Builtin.o PIDList.o
	$(CC) $(CFLAGS) -o smallsh smallsh.o Command.o DynamicArray.o Builtin.o PIDList.o

smallsh.o: smallsh.c Command.h DynamicArray.h
	$(CC) $(CFLAGS) -c smallsh.c

Command.o: Command.c Command.h Builtin.h 
	$(CC) $(CFLAGS) -c Command.c

Builtin.O: Builtin.c Builtin.h
	$(CC) $(CFLAGS) -c Builtin.c

PIDList.o: PIDList.c PIDList.h
	$(CC) $(CFLAGS) -c PIDList.c

DynamicArray.o: DynamicArray.c DynamicArray.h
	$(CC) $(CFLAGS) -c DynamicArray.c

clean:
	rm -f *.o $(TARGET) 
