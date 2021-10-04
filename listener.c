#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define EXAMPLE_PORT 6000
#define READ_GROUP "239.0.0.1"

int main(int argc, char** argv)
{
  struct sockaddr_in raddr;
  int addrlen, sockread, cnt;
  struct ip_mreq mreq;
  char message[50];

  /* set up sockets*/
  sockread = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockread < 0) {
    perror("socket");
    return(1);
  }
  
  int reuse = 1;
  if (setsockopt(sockread, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
  {
      perror("setsockopt(SO_REUSEADDR) failed");
  }
    
  if (setsockopt(sockread, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
  {
     perror("setsockopt(SO_REUSEPORT) failed");
  }
  
  bzero((char *)&raddr, sizeof(raddr));
  raddr.sin_family = AF_INET;
  raddr.sin_addr.s_addr = htonl(INADDR_ANY);
  raddr.sin_port = htons(EXAMPLE_PORT);
  addrlen = sizeof(raddr);
  raddr.sin_addr.s_addr = inet_addr(READ_GROUP);

  /* receive */
  int rc = 0;
  if (bind(sockread, (struct sockaddr *) &raddr, sizeof(raddr)) < 0) 
  {        
    perror("bind");
    return(1);
  }

  printf("rc: %d\n", rc);
  bzero(&mreq, sizeof(mreq));
  mreq.imr_multiaddr.s_addr = inet_addr(READ_GROUP);         
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  if (setsockopt(sockread, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) 
  {
    perror("setsockopt mreq");
    return(1);
  }

  while (1) 
  {
    printf("recv .,,\n");
    cnt = recvfrom(sockread, (void*)&message, sizeof(message), 0,  &raddr, &addrlen);
	printf("recvd .,,\n");
    if (cnt < 0) 
	  {
	    perror("recvfrom");
	    return(1);
	  } else if (cnt == 0) {
 	    break;
	  }
	 printf("%s: recvd message = \"%s\"\n", inet_ntoa(raddr.sin_addr), message);
  }
}