/*
    File: NetworkRequestChannel.H

    Author: Mishael Gonzalez
            Texas A&M University
    Date  : 7/3/2017
	CSCE 313-100

*/

#ifndef _NetworkRequestChannel_H_                   // include file only once
#define _NetworkRequestChannel_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <sstream>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include <string>

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   N e t w o r k R e q u e s t C h a n n e l */
/*--------------------------------------------------------------------------*/

class NetworkRequestChannel {

private:

  int fileDescriptor;

public:

  /* -- CONSTRUCTOR/DESTRUCTOR */
  
  NetworkRequestChannel(const string _server_host_name, const unsigned short _port_no);
  /* Creates a CLIENT-SIDE local copy of the channel. The channel is connected
	to the given port number at the given server host.
	THIS CONSTRUCTOR IS CALLED BY THE CLIENT. */

  NetworkRequestChannel(const unsigned short _port_no,
							void * (*connection_handler) (void *), int backlog);
  /* Creates a SERVER-SIDE local copy of the channel that is accepting connections
	at the given port number.
	NOTE that multiple clients can be connected to the same server-side end of the
	request channel. Whenever a new connection comes in, it is accepted by the
	the server, and the given connection handler is invoked. The parameter to
	the connection handler is the file descriptor of the slave socket returned
	by the accept call.
	NOTE that the connection handler does not want to deal with
	closing the socket. You will have to close the socket once the
	connection handler is done. */

  ~NetworkRequestChannel();
  /* Destructor of the local copy of the channel. */
  
  string send_request(string _request);
  /* Send a string over the channel and wait for a reply. */

  string cread();
  /* Blocking read of data from the channel. Returns a string of characters
     read from the channel. Returns NULL if read failed. */

  int cwrite(string _msg);
  /* Write the data to the channel. The function returns the number of characters written
     to the channel. */
	 
  int read_fd();
};


#endif


