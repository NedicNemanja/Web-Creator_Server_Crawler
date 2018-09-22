#ifndef ERRORCODES_H
#define ERRORCODES_H

#include <stdio.h>
#include <stdlib.h>

typedef enum ERRORCODE { OK=0,
                  ALLOCATION_FAIL=1,  //malloc() or realloc() returned NULL
                  NO_SERVING_PORT,
                  NO_COMMAND_PORT,
                  NO_NUM_THREADS,
                  NO_ROOT_DIR=5,
                  UNKNOWN_CMDARGUMENT,
                  READ_FAIL,  //read() failed to read
                  BAD_HTTPGET,  //got a http GET that was badly formated
                  SOCKET_CREATE,  //failed socket()
                  SOCKET_BIND=10,  //bind()
                  SOCKET_LISTEN, //listen()
                  SOCKET_ACCEPT, //accept()
                  PTHREAD_JOIN, //pthread_join returned error
                  UNRESPONSIVE_SERVING_SOCKET,  //closed or broken
                  SETSOCKOPT_REUSEADDR=15, //reuse sock addr failed
                  CANT_OPEN_FILE,
                  CANT_CLOSE_FILE,
} ERRORCODE;

//this informs about the error that terminated the prog abruptly
ERRORCODE ERROR;

//used after allocating/reallocating memory to check weather it was succesfull
#define NULL_Check(ptr) if(ptr == NULL) {fprintf(stderr, "Malloc/Realloc failed %s:%d\n", __FILE__, __LINE__); exit(ALLOCATION_FAIL);}

#endif
