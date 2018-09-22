#ifndef SERVER_H
#define SERVER_H

#include <time.h>

time_t CLOCK_START;

//socket(),bind(),listen() at port (reusing addressses with setsockopt)
void SBL_Socket(int* sock_ptr,int port);

#endif
