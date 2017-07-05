/* 
    File: dataserver.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/16
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <string>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>


#include <pthread.h>
#include <errno.h>

#include "NetworkRequestChannel.H"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* VARIABLES */
/*--------------------------------------------------------------------------*/

static int nthreads = 0;
const int MAX_MESSAGE = 255;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

//void handle_process_loop(RequestChannel & _channel);

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THREAD FUNCTIONS */
/*--------------------------------------------------------------------------*/
/*
void * handle_data_requests(void * args) {

  RequestChannel * data_channel =  (RequestChannel*)args;

  // -- Handle client requests on this channel. 
  
  handle_process_loop(*data_channel);

  // -- Client has quit. We remove channel.
 
  delete data_channel;
}
*/
/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- INDIVIDUAL REQUESTS */
/*--------------------------------------------------------------------------*/
string servRead(int * fileDescriptor){
	char buff[MAX_MESSAGE];
	read(*fileDescriptor, buff, MAX_MESSAGE);
	string s = buff;
	return s;
}

int servWrite(int * fileDescriptor, string message){
	if(message.length()>= MAX_MESSAGE){
		cout << "The message is huge" << endl;
	}
	if(write(*fileDescriptor, message.c_str(), message.length()+1) < 0){
		cout << "Error writing" << endl;
	}
}
//void process_hello(RequestChannel & _channel, const string & _request) {
  //_channel.cwrite("hello to you too");
//}
void process_hello(int * fileDescriptor, const string & _request){
	servWrite(fileDescriptor, "hello to you too");
}

//void process_data(RequestChannel & _channel, const string &  _request) {
  //usleep(1000 + (rand() % 5000));
  //_channel.cwrite("here comes data about " + _request.substr(4) + ": " + int2string(random() % 100));
  //_channel.cwrite(int2string(rand() % 100));
//}
void process_data(int * fileDescriptor, const string & _request){\
	usleep(1000 + (rand() % 5000));
	servWrite(fileDescriptor, int2string(rand() % 100));
}
/*
void process_newthread(RequestChannel & _channel, const string & _request) {
  int error;
  nthreads ++;

  // -- Name new data channel

  string new_channel_name = "data" + int2string(nthreads) + "_";
  //  cout << "new channel name = " << new_channel_name << endl;

  // -- Pass new channel name back to client

  _channel.cwrite(new_channel_name);

  // -- Construct new data channel (pointer to be passed to thread function)
  
  RequestChannel * data_channel = new RequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);

  // -- Create new thread to handle request channel

  pthread_t thread_id;
  //  cout << "starting new thread " << nthreads << endl;
  if (error = pthread_create(& thread_id, NULL, handle_data_requests, data_channel)) {
    fprintf(stderr, "p_create failed: %s\n", strerror(error));
  }  

}
*/
/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THE PROCESS REQUEST LOOP */
/*--------------------------------------------------------------------------*/

void process_request(int * fileDescriptor, const string & _request) {

  if (_request.compare(0, 5, "hello") == 0) {
    process_hello(fileDescriptor, _request);
  }
  else if (_request.compare(0, 4, "data") == 0) {
    process_data(fileDescriptor, _request);
  }
 // else if (_request.compare(0, 9, "newthread") == 0) {
   // process_newthread(_channel, _request);
  //}
  else {
    servWrite(fileDescriptor, "unknown request");
  }

}
/*
void handle_process_loop(RequestChannel & _channel) {

  for(;;) {

    cout << "Reading next request from channel (" << _channel.name() << ") ..." << flush;
    string request = _channel.cread();
    cout << " done (" << _channel.name() << ")." << endl;
    cout << "New request is " << request << endl;

    if (request.compare("quit") == 0) {
      _channel.cwrite("bye");
      usleep(10000);          // give the other end a bit of time.
      break;                  // break out of the loop;
    }

    process_request(_channel, request);
  }
  
}
*/
void* connection_handler(void * arg){
	int * fileDescriptor = (int*)arg;
	if(fileDescriptor == NULL){
		cout << "file descriptor = null" << endl;
	}
	cout << "Connection established" << endl;
	while(1){
		string req = servRead(fileDescriptor);
		if(req.compare("quit") == 0){
			servWrite(fileDescriptor, "bye");
			usleep(10000);
			break;
		}
		process_request(fileDescriptor, req);
	}
}
/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
	int backlog = 100;
	int B = 0;
	unsigned short portNum = 10100;
	unsigned short P= 0;
	
	int flag = 0;
	while((flag = getopt(argc, argv, "p:b:")) != -1){
		switch(flag){
			case 'p':
				P = atoi(optarg);
				if(P != 0){
					portNum = P;
				}
				else{
					cout << "Port number can't be 0, setting to default" << endl;
				}
				break;
			case 'b':
				B = atoi(optarg);
				if(B != 0){
					backlog = B;
				}
				else{
					cout << "Backlog can't be 0, setting to default" << endl;
				}
				break;
		}
	}
	cout << "Port number = " << portNum << endl;
	cout << "Backlog = " << backlog << endl;
	
  //  cout << "Establishing control channel... " << flush;
  NetworkRequestChannel server(portNum, connection_handler, backlog);
  //  cout << "done.\n" << flush;

  server.~NetworkRequestChannel();

}

