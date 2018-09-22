#include "Sockets.h"
#include "ErrorCodes.h"
#include "MyString.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>  //inet_aton
#include <netdb.h>  //gethostbyname
#include <unistd.h> //close
#include <stdlib.h>
#include <string.h> //memcpy
#include <stdio.h>

//socket(),bind(),listen() at port (reusing addressses with setsockopt)
void SBL_Socket(int* sock_ptr,int port){
  int sock = *sock_ptr;
  //create socket
  if( (sock=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    exit(SOCKET_CREATE);
  //make it that if the address is in use it will be reused and bind wont fail
  int opt=1;
  if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int))<0)
    exit(SETSOCKOPT_REUSEADDR);
  //bind socket
  struct sockaddr_in server1;
  struct sockaddr* server1ptr = (struct sockaddr*)&server1;
  server1.sin_family = AF_INET;                //IPv4 family
  server1.sin_addr.s_addr = htonl(INADDR_ANY); //bind to all interfaces
  server1.sin_port = htons(port);
  if( bind(sock, server1ptr, sizeof(server1)) < 0)
    exit(SOCKET_BIND);
  //listen for connections
  if( listen(sock,256) < 0)
    exit(SOCKET_LISTEN);

  *sock_ptr = sock;
}


//socket() and connect() at port of hostname. Return the socket fd
int SocketConnect(char* hostname,int port){
  //create socket
  int sock;
  if((sock=socket(AF_INET,SOCK_STREAM,0)) < 0)
    exit(SOCKET_FAIL);
  //get host(server) address
  struct sockaddr_in server;
  struct sockaddr* serverptr=(struct sockaddr*)&server;
  struct hostent* rem;
  struct in_addr host_address;

  if(IsIp(hostname)){
    //if host is IP convert the address in the hostname to binary form
    inet_aton(hostname,&host_address);
    rem = gethostbyaddr(&host_address,sizeof(host_address),AF_INET);
  }
  else{
    if((rem=gethostbyname(hostname)) == NULL){
        fprintf(stderr,"Cant respolve host name.\n");
        return -2;
    }
  }

  server.sin_family = AF_INET;
  memcpy(&server.sin_addr,rem->h_addr,rem->h_length);
  server.sin_port = htons(port);
  //connect to host
  if(connect(sock,serverptr,sizeof(server)) < 0){
    fprintf(stderr,"Host is unreachable. connect() failed.\n");
    return -1;
  }

  return sock;
}

/*
//socket() and connect() at port of hostname. Return the socket fd
int SocketConnect(char* hostname,int port){
  int sock;

  struct addrinfo hints;
  struct addrinfo* result, *rp;

  memset(&hints,0,sizeof(struct addrinfo));
  hints.ai_familty = AF_UNSPEC; //allow IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;

  if( (s = getaddrinfo(hostname,port,&hints,&result)) != 0)
    exit(GETADDRINFO_FAIL);

  //try each address of the list until a successfull connection is made
  for(rp=result; rp!=NULL; rp=rp->ai_next){
    if( (sock = socket(rp->ai_family,rp_>ai_soctype,rp->ai_protocol)) == -1)
      continue; //if sock failed
    if(connect(sock,rp->ai_addr,rp->ai_addrlen) != 1)
      break;    //if connected
    close(sock);  //else close sock and try again
  }

  if(rp==NULL)
    exit(CANT_CONECT);

  freeaddrinfo(result); //free list of addresses
  return sock;
}*/


//read all the contents of a socket
char* SocketRead(int sock){
  char* buffer = NULL;
  int bsize=0, val;

  do{
    //read READ_SIZE bytes every time (chunk by chunk)
    buffer = realloc(buffer,bsize+READ_SIZE);
    NULL_Check(buffer);
    if( (val = read(sock,buffer+bsize,READ_SIZE)) < 0)
      exit(READ_FAIL);
    bsize += val;
  }while(val>0);
  return buffer;
}
