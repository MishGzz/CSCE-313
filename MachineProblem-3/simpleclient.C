/* 
    File: simpleclient.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2013/01/31

    Simple client main program for MP3 in CSCE 313
	
	Mishael Gonzalez
	UIN: 725007329
	CSCE 313-100
	6/16/2017
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <iomanip>
#include <queue>
#include <sys/time.h>

#include "reqchannel.H"
#include "semaphore.H"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/
	struct response{ //response struct
		string str;
		int count;
		int requestedID;
		
		response(string st, int ri, int ct){
			str = st;
			int count = ct;
			requestedID = ri;
			
		}
	};
    struct boundedBuffer{ //buffer struct
		int size;
		Semaphore* lock;
		Semaphore* full;
		Semaphore* empty;
		queue<response> q;
		
		boundedBuffer(int s){
			size = s;
			lock = new Semaphore(1);
			full = new Semaphore(0);
			empty = new Semaphore(size);
		}
		~boundedBuffer(){
			delete lock;
			delete full;
			delete empty;
		}
		void addResponse(response r){
			empty -> P();
			lock->P();
			q.push(r); //critical section
			lock->V();
			full->V();
		}
		response getResponse(){
			full->P();
			lock->P();
			response r = q.front(); //critical section
			q.pop();
			lock->V();
			empty->V();
			return r;
		}
	};
	
	int numRequests = 100;//defaults
	int numWorkers = 10;
	int bufferSize = 400;
	
	int joesRequests = 0;
	int janesRequests = 0;
	int johnsRequests = 0;
	
	// 0 through 99
	vector<int> joesGraph(100);
	vector<int> janesGraph(100);
	vector<int> johnsGraph(100);
	
	boundedBuffer* buff;
	boundedBuffer* joesBuff;
	boundedBuffer* janesBuff;
	boundedBuffer* johnsBuff;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/
	
	int* joe = new int(0);
	int* jane = new int(1);
	int* john = new int(2);


/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* Helper Functions */
/*--------------------------------------------------------------------------*/


	void* requestThread(void* id){
		int requestID = *((int*)id);
		for(int i = 0; i < numRequests; i++){
			response* r = new response("place holder", requestID, 0);
			
			if(requestID == 0){
				joesRequests++;
				r->str = "data Joe Smith";
				r->requestedID = 0;
				r->count = joesRequests;
			}
			else if(requestID == 1){
				janesRequests++;
				r->str = "data Jane Smith";
				r->requestedID = 1;
				r->count = janesRequests;
			}
			else if(requestID == 2){
					johnsRequests++;
				r->str = "data John Doe";
				r->requestedID = 2;
				r->count = johnsRequests;
			}
			buff -> addResponse(*r);
			delete r;
		}
		cout << "requests for id: " << requestID << " completed" << endl;
		cout << "exiting request thread" << endl;
	}
	
	void* workerThread(void* rChannel){
		RequestChannel * channel = (RequestChannel*) rChannel;
		response r("", 0, 0);
		int c = 0;
		//cout << "WORKER THREAD" << endl;
		while(1){
				//cout << "in loop WORKER THREAD" << endl;
			r = buff->getResponse();
			//cout << "r = " << atoi(r.str.c_str()) << endl;
			if(r.str == "kill"){
				break;
			}
			
			string rep = channel->send_request(r.str);
			r.str = rep;
			
			if(r.requestedID == 0){
				//cout << "rep = " << atoi(r.str.c_str()) << endl;
				joesBuff -> addResponse(r);
			}
			else if(r.requestedID == 1){
				janesBuff -> addResponse(r);
			}
			else if(r.requestedID == 2){
				johnsBuff -> addResponse(r);
			}
		}
		channel->send_request("quit");
	}
	
	void* statThread(void* id){
		int requestedID = *((int*)id);
		response r("place holder", -1, -1);
		
		for(int i = 0; i < numRequests; i++){
			if(requestedID == 0){
				r = joesBuff->getResponse();
				//cout << "r  = " << r.str.c_str() << endl;
				joesGraph[atoi(r.str.c_str())]+=1;
			}
			else if(requestedID == 1){
				r = janesBuff->getResponse();
				//cout << "r  = " << atoi(r.str.c_str()) << endl;
				janesGraph[atoi(r.str.c_str())]+=1;
			}
			else if(requestedID == 2){
				r = johnsBuff->getResponse();
				//cout << "r  = " << r.str << endl;
				johnsGraph[atoi(r.str.c_str())]+=1;
			}
		}
		cout << "Statistics thread completed" << endl;
	}
	
	void seeHistogram(vector<int> d, string name){
	vector<int> a(10);
	for(int i = 0; i < a.size(); i++){
		for(int j = 0; j < a.size(); j++){
			a[i]+=d[j+i*10];
		}
	}
	cout << "\n " << name << endl <<setw(8)<<"0-9"<<setw(8)<<"10-19"<<setw(8)<<"20-29"<<setw(8)<<"30-39"<<setw(8)<<"40-49"<<setw(8)<<"50-59"
    <<setw(8)<<"60-69"<<setw(8)<<"70-79"<<setw(8)<<"80-89"<<setw(8)<<"90-99"<<endl<<endl;
	for(int i = 0; i < 10; i++){
		cout << setw(8) << a[i];
	}
	cout << endl;
	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
	cout << endl;
	}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
	
	cout << "Running" << endl;
	
	int flag;
	while((flag = getopt(argc, argv, "n:b:w:")) != -1){
		switch(flag){
			case 'n':
				numRequests = atoi(optarg);
				break;
			case 'b':
				bufferSize = atoi(optarg);
				break;
			case 'w':
				numWorkers = atoi(optarg);
				break;
			case '?':
				break;
			default:
				exit(0);
		}
	}
		cout << "Flags Received" << endl;
	
	cout << "Number of requests: " << numRequests << endl;
	cout << "Size of buffer: " << bufferSize << endl;
	cout << "Number of workers: " << numWorkers << endl;
	
	pthread_t requestThreads[3];
	pthread_t workerThreads[numWorkers];
	pthread_t statThreads[3];
	cout << "Threads have been initialized" << endl;
	
	buff = new boundedBuffer(bufferSize);
	joesBuff = new boundedBuffer(bufferSize);
	janesBuff = new boundedBuffer(bufferSize);
	johnsBuff = new boundedBuffer(bufferSize);
	cout << "Buffers have been created" << endl;
	
	pid_t pid = fork();
	if(pid == 0){
		execl("./dataserver", NULL, NULL);
	}
	else{
		//usleep(1000000);

		cout << "CLIENT STARTED:" << endl;

		cout << "Establishing control channel... " << flush;
		RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
		cout << "done." << endl;
		
		timeval start;
		timeval end;
		gettimeofday(&start, NULL);
		
		pthread_create(&requestThreads[0], NULL, requestThread, (void*)joe);
		pthread_create(&requestThreads[1], NULL, requestThread, (void*)jane);
		pthread_create(&requestThreads[2], NULL, requestThread, (void*)john);
		cout << "Request Threads have been created" << endl;
		
		for(int i = 0; i < numWorkers; i++){
			string rep = chan.send_request("newthread");
			RequestChannel* channel = new RequestChannel(rep, RequestChannel::CLIENT_SIDE);
			pthread_create(&workerThreads[i], NULL, workerThread, channel);
		}
		cout << "Worker Threads have been created SERVER_SIDE" << endl;
		
		pthread_create(&statThreads[0], NULL, statThread, (void*)joe);
		pthread_create(&statThreads[1], NULL, statThread, (void*)jane);
		pthread_create(&statThreads[2], NULL, statThread, (void*)john);
		cout << "Statistics Threads have been created" << endl;
	/*
	// -- Start sending a sequence of requests 

		string reply1 = chan.send_request("hello");
		cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;

		string reply2 = chan.send_request("data Joe Smith");
		cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << endl;

		string reply3 = chan.send_request("data Jane Smith");
		cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << endl;

		string reply5 = chan.send_request("newthread");
		cout << "Reply to request 'newthread' is " << reply5 << "'" << endl;
		RequestChannel chan2(reply5, RequestChannel::CLIENT_SIDE);

		string reply6 = chan2.send_request("data John Doe");
		cout << "Reply to request 'data John Doe' is '" << reply6 << "'" << endl;

		string reply7 = chan2.send_request("quit");
		cout << "Reply to request 'quit' is '" << reply7 << "'" << endl;

		string reply4 = chan.send_request("quit");
		cout << "Reply to request 'quit' is '" << reply4 << "'" << endl;
	*/
	
		pthread_join(requestThreads[0], NULL);
		pthread_join(requestThreads[1], NULL);
		pthread_join(requestThreads[2], NULL);
		cout << "Exiting request threads" << endl;
		
		response kill("kill", -5, -5);
		for(int i = 0; i < numWorkers; i++){
			buff->addResponse(kill);
		}
		for(int i = 0; i < numWorkers; i++){
			pthread_join(workerThreads[i], NULL);
		}
		cout << "terminated workers" << endl;
		
		pthread_join(statThreads[0], NULL);
		pthread_join(statThreads[1], NULL);
		pthread_join(statThreads[2], NULL);
		cout << "Exiting statistic threads" << endl;
		
		gettimeofday(&end,NULL);
		
		chan.send_request("quit");
		sleep(1);
		seeHistogram(joesGraph, "Joe Smith");
		seeHistogram(janesGraph, "Jane Smith");
		seeHistogram(johnsGraph, "John Doe");
		cout << "Running time:" << 1000000 * (end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec) << endl;
		cout << "Total requests:" << numRequests * 3 << endl;
		cout << "Total workers: " << numWorkers << endl;
		
		return 0;
	}
}
