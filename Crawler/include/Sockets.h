#ifndef SOCKETS_H
#define SOCKETS_H

#define READ_SIZE 64  //chunk size for read(...,...,READ_SIZE)

//socket(),bind(),listen() at port (reusing addressses with setsockopt)
void SBL_Socket(int* sock_ptr,int port);

//socket() and connect() at port of hostname. Return the socket fd
int SocketConnect(char* hostname,int port);

#endif
