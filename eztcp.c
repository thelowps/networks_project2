/** 
 *  eztcp.c
 *  Written by Andi Cescolini and David Lopes
 *  Computer Networks, 2013
 *  University of Notre Dame
 *
 *  A library to get the ugly TCP stuff out of the way.
 *  It is not intended to be extensible or flexible -- it makes
 *  a lot of assumptions. But it's the assumptions we want.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

int ezconnect (int* sock, char* ip, int port) {
  int s;
  struct sockaddr_in servaddr,cliaddr;
  
  // OPEN SOCKET
  s = socket(AF_INET,SOCK_STREAM,0);
  if (s == -1) {
    perror("Error when initializing the socket descriptor");
    return -1;
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(ip);
  servaddr.sin_port = htons(port);

  // CONNECT
  if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    perror("Error connecting to server");
    return -1;
  }

  *sock = s;
}

int ezsend (int sock, unsigned char* data, int len) {
  struct sockaddr_in servaddr;
  int s = sendto(sock, data, len, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
  if (s < 0) {
    perror("ERROR IN SEND");
  }
  return s;
}

int ezreceive (int sock, unsigned char* data, int len) {
  int socklen = (sizeof(struct sockaddr_in));
  int n = recvfrom(sock, data, len, 0, NULL, &socklen);
  if (n < 0) {
    perror("ERROR IN RECEIVE");
  } 
  return n;
}

int ezlisten (int* sock, int port) {
  int listenfd, connfd, n;
  struct sockaddr_in servaddr;

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  int optval = 1;
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
	     (const void *)&optval , sizeof(int));

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
  
  listen(listenfd, 1); // backlog of 1?

  *sock = listenfd;
}

int ezaccept (int sock, int* conn) {
  socklen_t clilen;
  struct sockaddr_in cliaddr;
  *conn = accept(sock, (struct sockaddr *)&cliaddr, &clilen); // TODO : error checking
}
