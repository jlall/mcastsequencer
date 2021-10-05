
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define EXAMPLE_PORT 6000
#define EXAMPLE_GROUP "239.0.0.1"

int main(int argc, char** argv)
{
   struct sockaddr_in addr;
   int addrlen, sock, cnt;
   struct ip_mreq mreq;
   char message[50];

   /* set up socket */
   sock = socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) 
   {
     perror("socket");
     return(1);
   }
   
   int reuse = 1;
   
   if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
     perror("setsockopt(SO_REUSEADDR) failed");

   if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
     perror("setsockopt(SO_REUSEPORT) failed");

   memset((char *)&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_port = htons(EXAMPLE_PORT);
   addrlen = sizeof(addr);

   if (argc > 1) 
   {
      /* send */
      addr.sin_addr.s_addr = inet_addr(EXAMPLE_GROUP);
      while (1) 
      {
	      time_t t = time(0);
	      sprintf(message, "t:%-24.24s", ctime(&t));
	      printf("sending: %s\n", message);
	      cnt = sendto(sock, message, sizeof(message), 0, (struct sockaddr *) &addr, addrlen);
	      if (cnt < 0) 
         {
 	         perror("sendto");
            return(1);
         }
	      sleep(2);
      }
   } 
   else 
   {
      /* receive */
      if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) 
      {        
         perror("bind");
         return(1);
      }    
      mreq.imr_multiaddr.s_addr = inet_addr(EXAMPLE_GROUP);         
      mreq.imr_interface.s_addr = htonl(INADDR_ANY);         
      if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) 
      {
	      perror("setsockopt mreq");
	      return(1);
      }         
      while (1) 
      {
         printf("waiting to recv.\n");
 	      cnt = recvfrom(sock, message, sizeof(message), 0, (struct sockaddr *) &addr, (unsigned int *)&addrlen);
	      if (cnt < 0) 
         {
	         perror("recvfrom");
	         return(1);
	      } 
         else if (cnt == 0) 
         {
 	         break;
	      }
	      printf("%s: message = \"%s\"\n", inet_ntoa(addr.sin_addr), message);
      }
    }
}

