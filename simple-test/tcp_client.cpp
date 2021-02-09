/*
 * tcp_client.cpp
 *
 *  Created on: 03-Feb-2021
 *      Author: Karthik Rao
 */



// TCP Client program
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#define PORT 5000
#define MAXLINE 1024
int main()
{
	int sockfd;
	char buffer[MAXLINE];
	char* message = "Hello Server";
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

	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, "[  \
    {\"name\":\"Ram\", \"email\":\"Ram@gmail.com\"},  \
    {\"name\":\"Bob\", \"email\":\"bob32@gmail.com\"} \
	]  ");
	write(sockfd, buffer, sizeof(buffer));
	printf("Message from server: ");
	read(sockfd, buffer, sizeof(buffer));
	puts(buffer);
	close(sockfd);
}


//#include <sys/socket.h>
//#include <sys/types.h>
//#include <netinet/in.h>
//#include <netdb.h>
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <errno.h>
//#include <arpa/inet.h>
//
//int main(void)
//{
//  int sockfd = 0,n = 0;
//  char recvBuff[1024];
//  struct sockaddr_in serv_addr;
//
//  memset(recvBuff, '0' ,sizeof(recvBuff));
//  if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
//    {
//      printf("\n Error : Could not create socket \n");
//      return 1;
//    }
//
//  serv_addr.sin_family = AF_INET;
//  serv_addr.sin_port = htons(5000);
//  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//
//  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
//    {
//      printf("\n Error : Connect Failed \n");
//      return 1;
//    }
//
//  while((n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
//    {
//      recvBuff[n] = 0;
//      if(fputs(recvBuff, stdout) == EOF)
//    {
//      printf("\n Error : Fputs error");
//    }
//      printf("\n");
//    }
//
//  if( n < 0)
//    {
//      printf("\n Read Error \n");
//    }
//
//  return 0;
//}
