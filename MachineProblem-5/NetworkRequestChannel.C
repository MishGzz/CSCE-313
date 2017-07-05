/* 
    File: NetworkRequestChannel.C

    Author: Mishael gonzalez
            Texas A&M University
    Date  : 7/3/2017
	CSCE 313-100
	

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


#include "NetworkRequestChannel.H"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    struct sockaddr_in servAddr;
	//struct sockaddr_in clientAddr;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */
	
/*--------------------------------------------------------------------------*/
/* HELPER FUNCTIONS */
/*--------------------------------------------------------------------------*/	

	int ClientConnector(const char * _server_host_name, const char * _port_no){
		struct sockaddr_in clientAddr;
		memset(&clientAddr, 0, sizeof(clientAddr));
		clientAddr.sin_family = AF_INET;
		
		if(struct servent * s = getservbyname(_port_no, "tcp")){
			clientAddr.sin_port = s->s_port;
		}
		else if((clientAddr.sin_port = htons((unsigned short)atoi(_port_no))) == 0){
			cout << "Couldn't connect to port" << endl;
		}
		if(struct hostent * m = gethostbyname(_server_host_name)){
			memcpy(&clientAddr.sin_addr, m->h_addr, m->h_length);
		}
		else if((clientAddr.sin_addr.s_addr = inet_addr(_server_host_name)) == INADDR_NONE){
			cout << "Couldn't determine host " << _server_host_name << endl; 
		}
		int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(sockfd < 0){
			cout << "Couldn't make socket" << endl;
		}
		int size = sizeof(clientAddr);
		if(::connect(sockfd, (struct sockaddr *)&clientAddr, (socklen_t)size) < 0){
			cerr << "error: " << errno << endl;
			cout << "Couldn't connect to host " << _server_host_name << " " << _port_no << endl; 
			cout << "sockfd: " << sockfd << endl;
			cout << "&clientAddr: " << &clientAddr << endl;
			cout << "sizeof(clientAddr): " << sizeof(clientAddr) << endl;
		}
		return sockfd;
	}
	
	int ServerConnector(const char * serv, int backlog){
		//struct sockaddr_in servAddr;
		memset(&servAddr, 0, sizeof(servAddr));
		servAddr.sin_family = AF_INET;
		servAddr.sin_addr.s_addr = INADDR_ANY;
		
		if(struct servent * s = getservbyname(serv, "tcp")){
			servAddr.sin_port = s->s_port;
		}
		else if((servAddr.sin_port = htons((unsigned short)atoi(serv))) == 0){
			cout << "Can't get " << serv << " serv entry" << endl;
		}
		int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(sockfd < 0){
			cout << "Couldn't make socket" << endl;
			exit(1);
		}
		if(::bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0){
			close(sockfd);
			cout << "Couldn't bind" << endl;
			exit(1);
		}
		if(::listen(sockfd, backlog) < 0){
			cout << "Couldn't listen" << endl;
			exit(1);
		}
		return sockfd;
	}
	
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   N e t w o r k R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

//client side
NetworkRequestChannel::NetworkRequestChannel(const string _server_host_name, const unsigned short _port_no){
	stringstream stream;
	stream << _port_no;
	string portNum = stream.str();
	
	//cout << "getting fd" << endl;
	fileDescriptor = ClientConnector(_server_host_name.c_str(), portNum.c_str());
	//cout << "got fd" << endl;
}

//server side
NetworkRequestChannel::NetworkRequestChannel(const unsigned short _port_no, void * (*connection_handler) (void *), int backlog){
	stringstream stream;
	stream << _port_no;
	string portNum = stream.str();
	
	int mstr = ServerConnector(portNum.c_str(), backlog);
	int size = sizeof(servAddr);
	
	while(1){
		int * slv = new int;
		pthread_t t;
		//pthread_attr_t attribute;
		//pthread_attr_init(&attribute);
		*slv = ::accept(mstr,(struct sockaddr*)&servAddr, (socklen_t*)&size);
		
		if(slv < 0){
			delete slv;
			
			if(errno == EINTR){
				continue;
			}
			else{
				cout << "error != EINTR" << endl;
			}
		}
		pthread_create(&t, NULL, connection_handler, (void*)slv);
	}
	cout << "Connected server side" << endl;
}

NetworkRequestChannel::~NetworkRequestChannel() {
	close(fileDescriptor);
}

/*--------------------------------------------------------------------------*/
/* READ/WRITE FROM/TO REQUEST CHANNELS  */
/*--------------------------------------------------------------------------*/

const int MAX_MESSAGE = 255;

int NetworkRequestChannel::read_fd(){
  return fileDescriptor;
}
  
string NetworkRequestChannel::send_request(string _request) {
  cwrite(_request);
  string s = cread();
  return s;
}

string NetworkRequestChannel::cread() {

  char buf[MAX_MESSAGE];

  if (::read(fileDescriptor, buf, MAX_MESSAGE) < 0) {
 // if (recv(fileDescriptor, buf, sizeof(buf), 0) < 0) {
    //perror(string("Request Channel (" + my_name + "): Error reading from pipe!").c_str());
	perror(string("Error reading").c_str());
  }
  
  string s = buf;

  //  cout << "Request Channel (" << my_name << ") reads [" << buf << "]\n";

  return s;

}

int NetworkRequestChannel::cwrite(string _msg) {
  if (_msg.length() >= MAX_MESSAGE) {
    cerr << "Message too long for Channel!\n";
    return -1;
  }

  //  cout << "Request Channel (" << my_name << ") writing [" << _msg << "]";

  const char * s = _msg.c_str();
  if (::write(fileDescriptor, s, strlen(s)+1) < 0) {
	  cout << "error" << endl;
  //if (send(fileDescriptor, s, strlen(s)+1, 0) < 0) {
    //perror(string("Request Channel (" + my_name + ") : Error writing to pipe!").c_str());
   perror(string("Error writing").c_str());
  }
  //  cout << "(" << my_name << ") done writing." << endl;
}


