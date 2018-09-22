#include <stdio.h>

#include "Server.h"
#include "Arguments.h"
#include "Threads.h"
#include "ErrorCodes.h"
#include "ServerCommands.h"

#include <sys/types.h>  /*socket(), bind(), listen(), accept(), setsockopt()..*/
#include <sys/socket.h>
#include <stdlib.h>     /*exit()*/
#include <arpa/inet.h>  /*htonl(), htons()*/
#include <unistd.h>     //close()

int main(int argc, char* argv[]){
  ParseArguments(argc,argv);
  CLOCK_START = time(NULL);

  //create serving and command sockets
  int serving_socket,command_socket;
  SBL_Socket(&serving_socket,SERVING_PORT);
  SBL_Socket(&command_socket,COMMAND_PORT);

  //create a thread pool to serve the connections
  pthread_t* ThreadPool = CreateThreadPool(NUM_THREADS, &serving_socket);

  ServeCommandSocket(command_socket);

  ShutdownThreads(ThreadPool,NUM_THREADS);

  free(ThreadPool);
  close(command_socket);
  close(serving_socket);

  return OK;
}

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
