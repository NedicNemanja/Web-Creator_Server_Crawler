#ifndef ERRORCODES_H
#define ERRORCODES_H

#include <stdio.h>
#include <stdlib.h>

typedef enum ERRORCODE { OK=0,
                  ALLOCATION_FAIL=1,  //malloc() or realloc() returned NULL
                  NO_SERVING_PORT,
                  NO_COMMAND_PORT,
                  NO_NUM_THREADS,
                  NO_SAVE_DIR=5,
                  NO_HOST,
                  NO_STARTING_URL,
                  UNKNOWN_CMDARGUMENT,
                  SOCKET_FAIL,
                  CANT_GET_HOST=10,
                  CONN_FAIL,
                  PTHREAD_JOIN,
                  SOCKET_CREATE,
                  SOCKET_BIND,
                  SOCKET_LISTEN=15,
                  SOCKET_ACCEPT,
                  SETSOCKOPT_REUSEADDR,
                  READ_FAIL,
                  CANT_FOPEN_FILE,
                  BAD_STARTLINK
} ERRORCODE;

//this informs about the error that terminated the prog abruptly
ERRORCODE ERROR;

//used after allocating/reallocating memory to check weather it was succesfull
#define NULL_Check(ptr) if(ptr == NULL) {fprintf(stderr, "Malloc/Realloc failed %s:%d\n", __FILE__, __LINE__); exit(ALLOCATION_FAIL);}

#endif
