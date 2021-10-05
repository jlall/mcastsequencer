/* ripped from multicast.c */
/* read then write with a seq number */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define EXAMPLE_PORT 6000
#define READ_GROUP "239.0.0.1"
#define WRITE_GROUP "239.0.0.2"

int main(int argc, char** argv)
{
  struct sockaddr_in saddr;
  struct sockaddr_in raddr;
  int saddrlen, raddrlen, sockread, sockwrite, cnt;
  struct ip_mreq mreq;
  char message[50];
  char smessage[50];

  /* set up sockets*/
  sockread = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockread < 0) {
    perror("socket");
    return(1);
  }
  sockwrite = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockwrite < 0) {
    perror("socket");
    return(1);
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

  printf("setting up some socket stuff\n");
  /* send */
  memset((void*)&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_port = htons(EXAMPLE_PORT);
  saddrlen = sizeof(saddr);
  saddr.sin_addr.s_addr = inet_addr(WRITE_GROUP);
 /* receive */
  memset((void*)&raddr, 0, sizeof(raddr));
  raddr.sin_family = AF_INET;
  raddr.sin_addr.s_addr = htonl(INADDR_ANY);
  raddr.sin_port = htons(EXAMPLE_PORT);
  raddrlen = sizeof(raddr);
  raddr.sin_addr.s_addr = inet_addr(READ_GROUP);
  uint n = 0;
  printf("ready to bind\n");
  /* receive */
	if (bind(sockread, (struct sockaddr *) &raddr, sizeof(raddr)) < 0) 
	{        
		perror("bind");
	 	return(1);
  }    
  printf("set up mcast listening\n");
  mreq.imr_multiaddr.s_addr = inet_addr(READ_GROUP);         
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);         
  if (setsockopt(sockread, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) 
	{
		perror("setsockopt mreq");
	 	return(1);
	}         
	printf("ready to go to while loop to listen\n");
  while (1) 
	{
   printf("...listening to: \n");
 	 cnt = recvfrom(sockread, message, sizeof(message), 0, (struct sockaddr *) &raddr, (unsigned int *)&raddrlen);
	 if (cnt < 0) 
	 {
	    perror("recvfrom");
	    return(1);
	 } else if (cnt == 0) 
   {
 	    printf("cnt = 0, breaking\n");
       break;
	 }
	 sprintf(smessage,"%i|%s",n++,message);
	 cnt = sendto(sockwrite, smessage, sizeof(smessage), 0, (struct sockaddr *) &saddr, saddrlen);
	 if (cnt < 0) 
	 {
 	   perror("sendto");
	   return(1);
	 }
	 printf("r:%s w:%s \"%s\"\n", inet_ntoa(raddr.sin_addr), inet_ntoa(saddr.sin_addr), smessage);
  }
}
