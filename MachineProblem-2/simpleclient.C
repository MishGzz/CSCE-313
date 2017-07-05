/* 
    File: simpleclient.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

    Simple client main program for MP2 in CSCE 313
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
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <errno.h>
#include <unistd.h>

#include "reqchannel.H"

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
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

//////////////////////////////////
void print_time_diff(struct timeval * tp1, struct timeval * tp2) {
  /* Prints to stdout the difference, in seconds and museconds, between two
     timevals. */

  long sec = tp2->tv_sec - tp1->tv_sec;
  long musec = tp2->tv_usec - tp1->tv_usec;
  if (musec < 0) {
    musec += 1000000;
    sec--;
  }
  printf(" [sec = %ld, musec = %ld] ", sec, musec);

}
///////////////////////////////////

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
	
  pid_t pid = fork();
  if(pid == 0){
	cout << "CLIENT STARTED:" << endl;

	cout << "Establishing control channel... " << flush;
	RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
	cout << "done." << endl;

	/* -- Start sending a sequence of requests */

	string reply1 = chan.send_request("hello");
	cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;

	string reply2 = chan.send_request("data Joe Smith");
	cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << endl;

	string reply3 = chan.send_request("data Jane Smith");
	cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << endl;

	struct timeval tp_start; /* Used to compute elapsed time. */
	struct timeval tp_end;
	for(int i = 0; i < 100; i++) {
		assert(gettimeofday(&tp_start, 0) == 0);///////////////////////////////////
		
		string request_string("data TestPerson" + int2string(i));
		string reply_string = chan.send_request(request_string);
		cout << "reply to request " << i << ":" << reply_string << endl;
		
		assert(gettimeofday(&tp_end, 0) == 0);/////////////////////////////////////
		print_time_diff(&tp_start, &tp_end);//////////////////////////////////////

	}
 
	string reply4 = chan.send_request("quit");
	cout << "Reply to request 'quit' is '" << reply4 << endl;
  }
  else{
	  execv("./dataserver", argv);
  }
  usleep(1000000);
}
