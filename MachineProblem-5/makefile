# makefile

all: dataserver client

semaphore.o: semaphore.H semaphore.C
	g++ -c -g semaphore.C
	
NetworkRequestChannel.o: NetworkRequestChannel.H NetworkRequestChannel.C
	g++ -c -g NetworkRequestChannel.C

dataserver: dataserver.C NetworkRequestChannel.o 
	g++ -g -o dataserver dataserver.C NetworkRequestChannel.o -lpthread -lnsl

client: client.C NetworkRequestChannel.o semaphore.o
	g++ -g -o client client.C NetworkRequestChannel.o semaphore.o -lpthread -lnsl
