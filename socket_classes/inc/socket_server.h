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
 * @file socket_server.h
 * @author karthik rao
 * @date 05 March 2021
 * @brief File is contails the class of Socket_server that inherits from Socket to
 * implement server features.
 */

#include "socket.h"
#include <netinet/in.h>
#include <signal.h>
#include <cstdlib>
#include <string>
#include <stddef.h>
#include <vector>
/**
 * The Socket_server class is inherited Socket class
 * The socket_server class extends the socket class to handle pub -sub pattern handling
 * It has the logic to
 * operate on a well known port
 * accept connections and record remote client socket details
 * run time update subscription details
 * connect to web services currently on udp

*/

class Socket_server : public Socket
{
public:
//! Constructor 
/*!
Handles the construction operation for the socket server
\param port : port definition 
\param : inform the construction routine if the type is tcp, udp or tcp_and_udp
*/
Socket_server(int port,std::string type_data);
//! desctructor
~Socket_server();
//! server_socket object initialization 
/*!
//! set up the port binding based on parameters passed. 
\return: void
*/
void server_init();
//! execute server operations
/*!
\return : value of socket operation 
          -1 is error
*/
int run_server();
//std::string packetData();
//! get pointer to application buffer
/*!
\return : void * generalized for different type of pointers
*/
void * packetData();
//! get packet size for socket operations
/*!
\return : size_t size of payload
*/
size_t packetSize(); 

//! get the remote side socket details via the socket structure 
/*!
\return : sockaddr_in standard socket structure
*/
sockaddr_in packetOrigin()
 {
      return cliaddr; 
 }
//! abstraction of the sendto() socket method
/*!
\param : ptr - pointer to application buffer
\param : sz - size of payload
\param : addr - socket structure
*/
bool sendPacketTo(const void *ptr, size_t sz, sockaddr_in addr) ;

//! backward compatility api for operations with open sound connect implementation.
/*
\return : size_t
*/
size_t get_address_length()  ;

//! tcp operation socket data 
/*!
\return : get the current tcp connected socket 
*/
int get_conn_fd()
{
     return connfd;
}

bool send_tcp_packet(const void *ptr, size_t sz, int addr) ;
socket_consts::socket_type get_connection_type()
{
     return current_connection_type;
}

private:
void bind_to_tcp();
void bind_to_udp();
void bind_to_socket();


int listenfd, connfd, udpfd,nready, maxfdp1;

fd_set rset;
ssize_t n;
socklen_t len;
//const int on = 1;
struct sockaddr_in cliaddr/*, servaddr*/;
std::string message = "Hello TCP/UDP Client(s)";

int msg_backlog = 10;
//char server_buffer[socket_consts::MAXLINE];
std::string server_buffer;

// for testing
socket_consts::socket_type current_connection_type;

};