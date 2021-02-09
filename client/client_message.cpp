/*
 * client_message.cpp
 *
 *  Created on: 09-Feb-2021
 *      Author: Chethan Deekshith
 */



// Serialization Client program
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "client_message.h"
#define PORT 5000
#define MAXLINE 1024
int main()
{
	int sockfd;
	char buffer[MAXLINE];
	const char* message = "Hello Server";
	struct sockaddr_in servaddr;

	int n, len;
	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket creation failed");
		exit(0);
	}

	memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(sockfd, (struct sockaddr*)&servaddr,
							sizeof(servaddr)) < 0) {
		printf("\n Error : Connect Failed \n");
	}

	Message obj;
    std::stringstream ss;
    obj.test_int_1 = 33;
    obj.test_str_1 = "TestSerialization";
    ss<<obj;

	write(sockfd, ss.str().c_str(), strlen(ss.str().c_str())+1);
	printf("Message from server: ");
	read(sockfd, buffer, sizeof(buffer));
	puts(buffer);
	close(sockfd);
}
