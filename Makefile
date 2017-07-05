# a simple makefile for compiling C++ files

CC= g++

all: main.o linked_list.o
	$(CC) main.o linked_list.o -o testlist

linkedlist.o: linked_list.h linked_list.cpp
	$(CC) -c linked_list.cpp

main.o: linked_list.h main.cpp
	$(CC) -c main.cpp

clean:
	/bin/rm -f *.o testlist
