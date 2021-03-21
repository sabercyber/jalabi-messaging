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
 * @file socket.cpp
 * @author karthik rao
 * @date 05 March 2021
 * @brief File implements all the functions declared for Socket class
 *     
 */



#include "socket.h"



Socket::Socket(std::string host = "127.0.0.1", int port= 5000, std::string type_req = "tcp"):
host(host), port(port)
{
    if (!strcmp(type_req.c_str(),"tcp"))
    {
        type = socket_consts::socket_type::tcp;
       
    }
    else if (!strcmp(type_req.c_str(),"udp"))
    { 
        type = socket_consts::socket_type::udp;   
    }
    else if (!strcmp(type_req.c_str(),"tcp_and_udp"))
    { 
        type = socket_consts::socket_type::tcp_and_udp;   
    }
    else
    {
        type = socket_consts::socket_type::udp;   
    }
#ifdef DEBUG_NETWORKING	
    std::cout << "Type of socket is " << type << std::endl;
#endif

    memset(&servaddr, 0, sizeof(servaddr));


    buffer.resize(socket_consts::MAXLINE);

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(host.c_str());
    
}


bool Socket::init(){
    bool status = false;
    if (type == socket_consts::socket_type::tcp){
     if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
#ifdef DEBUG_NETWORKING	
		std::cout << "socket creation TCP failed" << std::endl;
#endif
		return status;
     }
    //  bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	//  listen(sockfd, 10);

    }
    else{
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
#ifdef DEBUG_NETWORKING	
		std::cout << "socket creation UDP failed" << std::endl;
#endif
		return status;
     }
     //bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    }
    if (connect(sockfd, (struct sockaddr*)&servaddr,
							sizeof(servaddr)) < 0) {
#ifdef DEBUG_NETWORKING	
		std::cout << "Socket connect failed" << std::endl;
#endif
        return status;
	}
    status = true;
    return status;
}

Socket::~Socket(){
    close(sockfd);
}

bool Socket::send_data(const void * msg, size_t len)
{
    bool status = false;
    //buffer = msg;
    if (write(sockfd, msg, len) > 0)
        status = true;
    //close(sockfd);
    return status;

}

sockaddr_in& Socket::get_server_addr_data()
{
    return servaddr;
}







 size_t Socket::run()
{
    ssize_t bytes =-1;
    FD_SET(sockfd, &sock_set);
  
	// select the ready descriptor
#ifdef DEBUG_NETWORKING
    printf("Entering Select call\n");
#endif
    sock_ready = select((sockfd+1), &sock_set, NULL, NULL, NULL);

    // if tcp socket is readable then handle
    // it by accepting the connection
#ifdef DEBUG_NETWORKING	
    std::cout <<"New message received : " << sock_ready<< std::endl;
#endif
    if(type == socket_consts::socket_type::tcp)
    {
#ifdef DEBUG_NETWORKING	
    std::cout <<"TCP message receieved  : " << std::endl;
#endif
        if (FD_ISSET(sockfd, &sock_set))
        {
            sock_len = sizeof(servaddr);
            //tcp_connfd = accept(sockfd, (struct sockaddr*)&servaddr, &sock_len);
            
            
            bzero(&buffer[0], buffer.size());
#ifdef DEBUG_NETWORKING	
                printf("Message From TCP client: ");
#endif
            bytes = read(sockfd, &buffer[0],buffer.size());
#ifdef DEBUG_NETWORKING					
            std::cout <<"Message receieved : " << buffer<< std::endl;
#endif
            // bytes = write(connfd, (const char*)message.c_str(),buffer.size());
            // close(connfd);
        }
    }
    // if udp socket is readable receive the message.
    else
    {
        if (FD_ISSET(sockfd, &sock_set)) 
        {
            sock_len = sizeof(servaddr);
            bzero(&buffer[0], buffer.size());
#ifdef DEBUG_NETWORKING	
            printf("\nMessage from UDP client: ");
#endif
            bytes = recvfrom(sockfd,&buffer[0], buffer.size(), 0,
                        (struct sockaddr*)&servaddr, &sock_len);
#ifdef DEBUG_NETWORKING	
            std::cout <<"Message receieved : " << buffer<< std::endl;
#endif
            // sendto(sockfd, (const char*)message.c_str(), buffer.size(), 0,
            //     (struct sockaddr*)&servaddr, sizeof(servaddr));
        }
    }
    return bytes;
}



void * Socket::packetData() 
{
	 return buffer.empty() ? 0 : &buffer[0];
	 
}

size_t Socket::packetSize() 
{ 
	  return buffer.size(); 
}
