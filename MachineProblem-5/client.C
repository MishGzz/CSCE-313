/* 
    File: client.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

    Simple client main program for MPs in CSCE 313
	
	Mishael Gonzalez
	UIN: 725007329
	CSCE 313-100
	6/30/2017
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
#include <pthread.h>
#include <sys/socket.h>


#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <iomanip>
#include <queue>
#include <sys/time.h>
#include <signal.h>

#include "NetworkRequestChannel.H"
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
			count = ct;
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
	
	int numRequests = 1500;//defaults
	int numWorkers = 16;
	int bufferSize = 510;
	string Host = "127.0.0.1";
	int portNum = 23500;
	
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
	
	volatile sig_atomic_t histograms = false; /*BONUS*/
	const int time_int = 1000;/*BONUS*/
	
	vector<NetworkRequestChannel*> NetworkChanns;
	int* id;

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
	/*BONUS*/
	void histogramsWanted(int s){
		histograms = true;
	}
	/*BONUS*/

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
	
	void* eventThreadHandler(void* a){
		//RequestChannel* channs[numWorkers];
		id = new int[numWorkers];
		response r("", 0, 0);
		int maximum = 0;
		fd_set rfds;
		struct timeval tv = {0, 10};
		int retval;
		int writes = 0;
		int reads = 0;
		int totalRequests = numRequests*3;
		
		//RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
		
		for(int i = 0; i < numWorkers; i++){
			//string rep = chan.send_request("newthread");
			//RequestChannel* channel = new RequestChannel(rep, RequestChannel::CLIENT_SIDE);
			//channs[i] = channel;
			//channs[i] = new RequestChannel(rep, RequestChannel::CLIENT_SIDE);
			NetworkRequestChannel* channel = new NetworkRequestChannel(Host, portNum);
			NetworkChanns.push_back(channel);
			id[i] = -1; //no one
		}
		
		//fill channels
		for(int i = 0; i < numWorkers; i++){
			
			r = buff->getResponse();
			//cout << "hey 1" << endl;
			writes += 1;
			//cout << Network
			NetworkChanns[i] -> cwrite(r.str);
			//cout << "hey 2" << endl;
			id[i] = r.requestedID;
			//cout << "hey 3" << endl;

		}
		
		while(1){
			/*BONUS*/
			//periodically print histograms
			if(histograms){
				seeHistogram(joesGraph, "Joe Smith");
				seeHistogram(janesGraph, "Jane Smith");
				seeHistogram(johnsGraph, "John Doe");
				cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
				histograms = false;
			}
			/*BONUS*/
			FD_ZERO(&rfds);
			
			for(int i = 0; i < numWorkers; i++){
				if(NetworkChanns[i] -> read_fd() > maximum){
					maximum = NetworkChanns[i] -> read_fd();
				}
				FD_SET(NetworkChanns[i]->read_fd(), &rfds);
			}
			retval = select(maximum + 1, &rfds, NULL, NULL, &tv);
			if(retval == -1){
				perror("select()");
			}
			if(retval){
				for(int i = 0; i < numWorkers; i++){
					if(FD_ISSET(NetworkChanns[i]->read_fd(), &rfds)){ // channs[i] is ready to read
						string rep = NetworkChanns[i]->cread();
						reads += 1;

						int ID = id[i];
						if(ID == 0){
							joesBuff->addResponse(response(rep, ID, 0));
						}
						else if(ID == 1){
							janesBuff->addResponse(response(rep, ID, 0));
						}
						else if(ID == 2){
							johnsBuff->addResponse(response(rep, ID, 0));
						}
						if(writes < totalRequests){//shouldnt be able to write more than what was requested
							r = buff->getResponse();
							writes += 1;
							NetworkChanns[i]->cwrite(r.str);
							id[i] = r.requestedID;
						}
					}
				}
			}
			if(reads == totalRequests){
				break;
			}
		}
		//now the request channels must be closed
		for(int i = 0; i < numWorkers; i++){
			cout <<"test" << endl;
			NetworkChanns[i]->cwrite("quit");
		}
		//chan.send_request("quit");
		//return 0;
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

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
	
	cout << "Running" << endl;
	string tmp;
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
				if(numWorkers >= 125){
					cout << "To many request channels" << endl;
					cout << "Replacing with" << 120 << endl;
					numWorkers = 120;
				}
				break;
			case 'h':
				tmp = optarg;
				if(tmp != ""){
					Host = tmp;
				}
				else{
					cout << "Host name set to default" << endl;
				}
				break;
			case 'p':
				portNum = atoi(optarg);
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
	cout << "Number of request channels to be handled by event handler thread: " << numWorkers << endl;
	
	pthread_t requestThreads[3];
	pthread_t eventHandler;
	pthread_t statThreads[3];
	cout << "Threads have been initialized" << endl;
	
	buff = new boundedBuffer(bufferSize);
	joesBuff = new boundedBuffer(bufferSize);
	janesBuff = new boundedBuffer(bufferSize);
	johnsBuff = new boundedBuffer(bufferSize);
	cout << "Buffers have been created" << endl;
	
	//pid_t pid = fork();
	//if(pid == 0){
	//	execl("./dataserver", NULL, NULL);
	//}
	//else{

	  cout << "CLIENT STARTED:" << endl;
	  
	
	  //cout << "Establishing control channel... " << flush;
	  //RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
	  //cout << "done." << endl;;
	  
	  /*BONUS*/
	  signal(SIGALRM, histogramsWanted);
	  struct itimerval timer;
	  timer.it_value.tv_sec = time_int/1000;
	  timer.it_value.tv_usec = (time_int*1000) % 1000000;
	  timer.it_interval = timer.it_value;
	  if(setitimer(ITIMER_REAL, &timer, NULL) == -1){
		  perror("error calling setitimer()");
	  }
	  /*BONUS*/

	  
		timeval start;
		timeval end;
		gettimeofday(&start, NULL);
		
		pthread_create(&requestThreads[0], NULL, requestThread, (void*)joe);
		pthread_create(&requestThreads[1], NULL, requestThread, (void*)jane);
		pthread_create(&requestThreads[2], NULL, requestThread, (void*)john);
		cout << "Request Threads have been created" << endl;
		
		pthread_create(&eventHandler, NULL, eventThreadHandler, NULL); //request channel created in eventHandler
		cout << "Event Handler Thread has been created" << endl;
			
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
		pthread_join(requestThreads[2], NULL);//wait
		cout << "Exiting request threads" << endl;
		
		pthread_join(eventHandler, NULL);//wait
		cout << "Exiting event handler" << endl;
		
		pthread_join(statThreads[0], NULL);
		pthread_join(statThreads[1], NULL);//wait
		pthread_join(statThreads[2], NULL);
		cout << "Exiting statistic threads" << endl;
		
		gettimeofday(&end,NULL);
			
		//chan.send_request("quit");
		sleep(1);
		seeHistogram(joesGraph, "Joe Smith");
		seeHistogram(janesGraph, "Jane Smith");
		seeHistogram(johnsGraph, "John Doe");
		cout << "Running time:" << 1000000 * (end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec) << endl;
		cout << "Total requests:" << numRequests * 3 << endl;
		cout << "Total threads handled by event handler: " << numWorkers << endl;
		
		for(int i = 0; i < NetworkChanns.size(); i++){
			delete NetworkChanns[i];
		}
			
		return 0;
	//}
	//*** find . -type p -delete***
  usleep(1000000);
}