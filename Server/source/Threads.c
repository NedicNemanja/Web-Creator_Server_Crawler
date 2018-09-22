#include "Threads.h"
#include "ErrorCodes.h"
#include "Http.h" /*HttpGET*/
#include "SharedGlobals.h"

#include <pthread.h>
#include <stdlib.h> /*malloc(), */
#include <sys/types.h>  /*accept() */
#include <sys/socket.h>
#include <unistd.h> //close()
#include <signal.h> //sigaction() pthread_kill()
#include <poll.h>

#include <stdio.h>
#include <string.h> //memcpy
#include <signal.h>

//init counter mutex
pthread_mutex_t couter_lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

/*When serving_socket becomes available, threads will wake up from poll and try
to accept the connection. This causes undefined behaviour as all threads wake up
but only one(?) will accept.
In order to tackle this polling is locked by poll_lock mutex. Now only 1 thread
can be expecting a connection at every time and when serving socket becomes
available only that one will be able to poll and accept.*/
pthread_mutex_t poll_lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;


/*CAUTION: serving_socket must be passed as a pointer because this
thread creator might die before all threads have initialized their
serving socket.*/
pthread_t* CreateThreadPool(int num_threads, int* serving_socket_ptr){
  pthread_t* ThreadPool = malloc(sizeof(pthread_t)*num_threads);
  for(int i=0; i<num_threads; i++){
    pthread_create(&ThreadPool[i],0,Thread,serving_socket_ptr);
  }
  return ThreadPool;
}

/*EVERYTHING HERE MUST BE THREAD SAFE*/
void* Thread(void* serving_socket_ptr){
  printf("thread start: %d\n", (int)pthread_self());
  int serving_socket = *(int*)serving_socket_ptr;

  //in case the thread is blocked by poll, unblock it so it can shutdown
  SetShutDownHandler();

  while(SHUTDOWN_FLAG != 1){
    pthread_mutex_lock(&poll_lock);
    //block until a connection is available or a signal interrupts
    struct pollfd poll_Sock = { .fd=serving_socket,.events=POLLIN,.revents=0};
    int val;
    if(SHUTDOWN_FLAG != 1){
      //printf("poll block.%d\n", (int)pthread_self());
      val = poll(&poll_Sock, (nfds_t)1, -1);
      //printf("poll unblock.%d\n", (int)pthread_self());
    }
    pthread_mutex_unlock(&poll_lock);

    if(SHUTDOWN_FLAG!=1){
      if(val == POLLIN){
        //connection available
        int newsock;
        signal(SIGPIPE,SIG_IGN);  //in case conn is closed
        if( (newsock=accept(serving_socket,NULL,NULL)) < 0)
          exit(SOCKET_ACCEPT);

        HttpGET Get = { .url=NULL };
        //Read Http GET Request from newsocket
        if(HttpGetRequest(newsock,&Get) == 0){
          printf("%d GET: %s\n", (int)pthread_self(), Get.url);
          /*Answer: (load page in a string and give that string to HttpAnswer to
          send), also answers 403,404,500 errors*/
          HttpAnswer(newsock,LoadPage(Get.url));
        }

        FreeHttpGET(&Get);
        close(newsock);
        signal(SIGPIPE,SIG_DFL);
      }
      else if(poll_Sock.revents == POLLHUP ||
              poll_Sock.revents == POLLERR ||
              poll_Sock.revents == POLLNVAL){
        //this socket might be closed or broken
        exit(UNRESPONSIVE_SERVING_SOCKET);
      }
    }
  }
  printf("thread shutdown: %d\n", (int)pthread_self());
}

void ShutdownThreads(pthread_t* ThreadPool,int num_threads){
  //send a signal for threads to shutdown
  for(int i=0; i<num_threads; i++){
    pthread_kill(ThreadPool[i],SIGUSR1);
  }
  //wait for threads to finish
  for(int i=0; i<num_threads; i++){
    if(pthread_join(ThreadPool[i],NULL)!=0)
      exit(PTHREAD_JOIN);
  }
}

void SetShutDownHandler(){
  struct sigaction saction;
  saction.sa_handler = &shutdown_handler;
  sigemptyset(&saction.sa_mask);
  saction.sa_flags = 0;
  sigaction(SIGUSR1,&saction,NULL);
}

void shutdown_handler(int signum){
  //do absolutely nothing, this is only so poll() is interrupted
}
