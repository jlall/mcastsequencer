/* ripped from multicast.c */
/* 
this is a node that listens to the seq stream and writes to the 
unseq stream ( but no writing yet )
*/
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>

#define EXAMPLE_PORT 6000
#define WRITE_GROUP "239.0.0.1"
#define READ_GROUP "239.0.0.2"

main(int argc)
{
  struct sockaddr_in saddr;
  struct sockaddr_in raddr;
  int addrlen, sockread, sockwrite, cnt;
  struct ip_mreq mreq;
  char message[50];

  /* set up sockets*/
  sockread = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockread < 0) {
    perror("socket");
    exit(1);
  }
  sockwrite = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockwrite < 0) {
    perror("socket");
    exit(1);
  }

  int reuse = 1;
  if (setsockopt(sockread, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");

  if (setsockopt(sockread, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
    perror("setsockopt(SO_REUSEPORT) failed");

  if (setsockopt(sockwrite, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");

  if (setsockopt(sockwrite, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
    perror("setsockopt(SO_REUSEPORT) failed");

  /* send */
  bzero((char *)&saddr, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_port = htons(EXAMPLE_PORT);
  addrlen = sizeof(saddr);
  saddr.sin_addr.s_addr = inet_addr(WRITE_GROUP);

  bzero((char *)&raddr, sizeof(raddr));
  raddr.sin_family = AF_INET;
  raddr.sin_addr.s_addr = htonl(INADDR_ANY);
  raddr.sin_port = htons(EXAMPLE_PORT);
  addrlen = sizeof(raddr);
  raddr.sin_addr.s_addr = inet_addr(READ_GROUP);

  /* receive */
	if (bind(sockread, (struct sockaddr *) &raddr, sizeof(raddr)) < 0) 
	{        
		perror("bind");
	 	exit(1);
  }    

  mreq.imr_multiaddr.s_addr = inet_addr(READ_GROUP);         
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);         
  if (setsockopt(sockread, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) 
	{
		perror("setsockopt mreq");
	 	exit(1);
	}         
	while (1) 
	{
 	 cnt = recvfrom(sockread, message, sizeof(message), 0, 
			(struct sockaddr *) &raddr, &addrlen);
	 if (cnt < 0) 
	 {
	    perror("recvfrom");
	    exit(1);
	 } else if (cnt == 0) {
 	    break;
	 }
/*
	 cnt = sendto(sockwrite, message, sizeof(message), 0, (struct sockaddr *) &saddr, addrlen);
	 if (cnt < 0) 
	 {
 	   perror("sendto");
	   exit(1);
	 }
*/
	 printf("%s: sent message = \"%s\"\n", inet_ntoa(raddr.sin_addr), message);
  }
}
