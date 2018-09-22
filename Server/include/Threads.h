#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>

#include "ErrorCodes.h"
#include "Http.h"

pthread_mutex_t counter_lock;


pthread_t* CreateThreadPool(int num_threads, int* serv_sock_ptr);

void* Thread(void* serving_socket_ptr);

//send SIGUSR1 to threads so the shutdown and wait for them
void ShutdownThreads(pthread_t* ThreadPool,int num_threads);

//used by thread to set up a handler for when they shutdown
void SetShutDownHandler();
void shutdown_handler(int signum);
#endif
