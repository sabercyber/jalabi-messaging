
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
 * @file socket_server.cpp
 * @author karthik rao
 * @date 05 March 2021
 * @brief File implements class of Socket_server which is inherited from Socket
 *     
 */

#include "socket_server.h"

Socket_server::Socket_server(int port,std::string type_data)
: Socket("",port,type_data)
{
    Socket::servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_buffer.resize(socket_consts::MAXLINE);
}
    

Socket_server::~Socket_server()
{
    
}


void Socket_server::bind_to_tcp()
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0 );
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	listen(listenfd, 10);
}

void Socket_server::bind_to_udp()
{
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
	// binding server addr structure to udp sockfd
	bind(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
}

void Socket_server::bind_to_socket()
{
    bind_to_tcp();
    /* create UDP socket */
    bind_to_udp();
	
}
int Socket_server::run_server()
{

    //for (;;)
	 {

		// set listenfd and udpfd in readset
        if (Socket::type == socket_consts::socket_type::tcp_and_udp)
        {
		    FD_SET(listenfd, &rset);
            FD_SET(udpfd, &rset);
           
        }
		else if(Socket::type == socket_consts::socket_type::udp)
        {
            FD_SET(udpfd, &rset);
            
        }  
        else if(Socket::type == socket_consts::socket_type::tcp)
        {
            FD_SET(listenfd, &rset);
            
        }

		// select the ready descriptor
#ifdef DEBUG_NETWORKING
		printf("Entering Select call\n");
#endif
		nready = select(maxfdp1, &rset, NULL, NULL, NULL);

		// if tcp socket is readable then handle
		// it by accepting the connection
		if (FD_ISSET(listenfd, &rset)) {
            current_connection_type = socket_consts::socket_type::tcp;
			len = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);
			
			
				bzero(&server_buffer[0], server_buffer.size());
#ifdef DEBUG_NETWORKING	
				printf("Message From TCP client: ");
#endif
				read(connfd, &server_buffer[0],server_buffer.size());
#ifdef DEBUG_NETWORKING					
				std::cout <<"Message receieved : " << server_buffer<< std::endl;
#endif
				write(connfd, (const char*)message.c_str(),message.length());
                ///conn_fd_list.push_back(connfd);
				// close(connfd);
                // sendto(connfd, (const char*)message.c_str(), message.length(), 0,
				//  (struct sockaddr*)&cliaddr, sizeof(cliaddr));

                 //(message.c_str(),message.length(), cliaddr);
				
			
			
		}
		// if udp socket is readable receive the message.
		if (FD_ISSET(udpfd, &rset)) {
           current_connection_type = socket_consts::socket_type::udp;
			len = sizeof(cliaddr);
			bzero(&server_buffer[0], server_buffer.size());
#ifdef DEBUG_NETWORKING	
			printf("\nMessage from UDP client: ");
#endif
			n = recvfrom(udpfd,&server_buffer[0], server_buffer.size(), 0,
						(struct sockaddr*)&cliaddr, &len);
#ifdef DEBUG_NETWORKING	
			std::cout <<"Message receieved : " << server_buffer<< std::endl;
#endif
			sendto(udpfd, (const char*)message.c_str(), message.length(), 0,
				(struct sockaddr*)&cliaddr, sizeof(cliaddr));
                
		}
	}

}


void Socket_server::server_init()
{
	// clear the descriptor set
	FD_ZERO(&rset);

    if (Socket::type == socket_consts::socket_type::tcp)
    {
        bind_to_tcp();
        maxfdp1 = listenfd +1;
       
    }
    else if (Socket::type == socket_consts::socket_type::udp)
    {
        bind_to_udp();
        maxfdp1 = udpfd +1;
        
       
    }
    else 
    {
        bind_to_socket();
        maxfdp1 = std::max(listenfd, udpfd) + 1;
        
    }

}




// std::string Socket_server::packetData() 
// {
// 	 //return server_buffer.empty() ? 0 : &server_buffer[0];
// 	 return server_buffer;
// }

void * Socket_server::packetData() 
{
	 return server_buffer.empty() ? 0 : &server_buffer[0];
	 
}

size_t Socket_server::packetSize() 
{ 
	  return server_buffer.size(); 
}


bool Socket_server::sendPacketTo(const void *ptr, size_t sz, sockaddr_in addr) 
  {
    

    int sent = 0;
    
    int res;
    
       
    
        // TODO: need to be able to set to both tcp and udp
        res = sendto(udpfd, ptr, sz, 0, (struct sockaddr *)&addr, get_address_length());
		std::cout << "data sent " << res << std::endl;
#ifdef WIN32
      if (res == -1 && WSAGetLastError() == WSAEINTR) continue;
      else sent = res;
#else
      //if (res == -1) cerr << "sendto handle=" << handle << ", res:" << res << ", sz=" << sz << ", errno=" << errno << " " << strerror(errno) << "\n";
      //if (res == -1 && errno == EINTR) continue;
      //else sent = res;
#endif
    

    return (size_t)sent == sz;
  }

bool Socket_server::send_tcp_packet(const void *ptr, size_t sz, int addr) 
{
    bool status = false;
     if (write(addr, (const char*)ptr,sz) > 0)
        status = true;

    return status;
    
}


  size_t Socket_server::get_address_length()  {

    //if (cliaddr.sin_family == AF_UNSPEC) return 0;
    if (cliaddr.sin_family == AF_INET6) return sizeof(struct sockaddr_in6);
    else  return sizeof(struct sockaddr_in);
    
   
  }