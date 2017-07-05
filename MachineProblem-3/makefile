# makefile

all: dataserver simpleclient

semaphore.o: semaphore.H semaphore.C
	g++ -c -g semaphore.C

reqchannel.o: reqchannel.H reqchannel.C
	g++ -c -g reqchannel.C

dataserver: dataserver.C reqchannel.o semaphore.o 
	g++ -g -o dataserver dataserver.C reqchannel.o semaphore.o -lpthread

simpleclient: simpleclient.C reqchannel.o semaphore.o
	g++ -g -o client simpleclient.C reqchannel.o semaphore.o -lpthread
