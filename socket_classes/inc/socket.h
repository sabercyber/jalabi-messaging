/****************************************************************************
 * Copyright (C) 2021 by Karthik Rao (sabercyber)                                  *
 *                                                                          *
 * This file is part of C++ socket classes .                                                *
 *                                                                          *
 *   MIT License



Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 ****************************************************************************/

/**
 * @file socket.h
 * @author karthik rao
 * @date 05 March 2021
 * @brief file is the basis of abstration of socket interface. This is does to simplify
 * Socket operation use to as shown in client_app.cpp.
 * It looks something as shown below
 *      @param1 is the host address
 *      @param2 is the port operation 
 *      @param3 is the protocol. tcp or udp
 * 
 *      Socket local_socket("127.0.0.1", 5000, tcp);
        local_socket.init();
        local_socket.send_data("From class sockets");
 */


#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>


namespace socket_consts {

  const int MAXLINE = 2048;
  enum socket_type {
      tcp=1,
      udp,
      tcp_and_udp // reserved for server operation 
  };
}

/** Class Socket handles the basics of socket operations. 
 * it is a abstraction of the C socket functions 
 * This class intentionally is designed to handle both TCP and UDP. 
 * The regular design would have something like this which I have done earlier
 *            ---- tcp_socket
 * socket ---|
 *            ---- udp_socket
 * This socket class is defined with the implied inheritence for the server which may handle
 * both tcp and udp as this class is seen as configurable as needed based on type. 
 * This class is designed looking a server needs and looking at an alternative design for the client
 * implementation. 
 * This is also experimental where in maintenance would be complicated from what the gurus state
 * I however this is something set in stone that would not be touched once the fundamental testing
 * completes
 * The pattern of combining the server method of handling both and interchange of message has
 * had the overiding decision in the this class design.
 *  */



class Socket {
public:
//! Socket Constrution 
/*!
The socket construct creates a socket object for managing socket operations
\ param host is a string to identify host address. If you are not going outside
  the network its local host
\ param port is the port identificaiton. This is the well known server port. 
\ param type is the type socket protocol identification options are
  tcp : for tcp operation 
  udp : for udp operation
  This selection is based on server operational capability and design reqiorement.
*/
Socket(std::string host, int port, std::string type);
//! Initalization 
/*!
Initialization routine to check on correct initialization method with feedback for failure. 
This is inplace because a failure is construction is unknown. This completes the setup process
and is to called after instantiation of hte object. 
\return book : true (success), false (failure)
*/

bool init();
//! desctructor
/*!
No major operation for the destructor. Just prevting compiler defined destructor from being used. 
*/
virtual ~Socket();
//! method to send data to via setup socket operation. Absctracted method for the send
/*
\param msg : The payload to be sent to the remote side
\param len : lenght of the payload for socket buffer calculations
\return bool for sucess or failure of the operation 
*/
bool send_data(const void* msg, size_t len);

//! simple function the stored reference of sockaddr_in. This structure has the required data for remote client operation 
/*!

\ return sockaddr_in reference the socket struct requred. 
*/
sockaddr_in & get_server_addr_data(); 

//! start the operation if you need the class manage incoming data
/*!
This function is basically a method to get into the select call and handle
every return. The business logic is to be handled on the return. 
Use decoding classes to retrive message recieved to decode payload and take action. 
\return size_t : return current socket value that exited the select call
*/
size_t run();
//! supoort fuction to return the application level buffer to extract or write data
/*!
Inherited classes can overide the base class based on its requirement as its 
buffers may be different. 
\ return void * the pointer to application bufer
*/

virtual void * packetData() ;

//! supoort fuction to return the size of application level buffer
/*!
Inherited class can override function based its use. 
\ return size_t size of application buffer
*/
virtual size_t packetSize() ;

/*!
* The class uses protected protection instead of usual private. This is done as
we will inherit the server operaion and will need access to fundamental data to prevent
duplication. 
*/
protected:
/*!
< sockfd holds the valid socket value that is in operation 
*/
int sockfd;
/*!
< tcp_connfd for tcp connection, the accept() socket value
*/
int tcp_connfd;
/*!
< buffer: the application level buffer to store payload coming 
from up the socket stack or send it down the socket stack. 
*/
std::string buffer;
/*!
< servaddr stores the remote side socket data
*/
struct sockaddr_in servaddr;
/*!
< host stores the host address as identifed during design
*/
std::string host;
/*!
< type holds the value of type of socket being tcp or udp. This is used 
for determing control flow during the operation.
*/
socket_consts::socket_type type;
/*!
< port the remote side port of operations
*/
int port;
/*!
< sock_set is the fd_set which is os support to hold multiple socket types 
useful during the select call to group. Though technically not required as 
we operate with one ephimaral port but is kept for convention
*/
fd_set sock_set;
/*!
< sock_ready used for select call return 
*/
int sock_ready;
/*!
< sock_len used in socket operation for UDP operation to extract data of servaddr struct
*/
socklen_t sock_len;

};