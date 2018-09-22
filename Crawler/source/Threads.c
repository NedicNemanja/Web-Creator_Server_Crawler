#include "Threads.h"
#include "ErrorCodes.h"
#include "LinkQueue.h"
#include "Sockets.h"
#include "Http.h"
#include "MyString.h"
#include "HtmlPage.h"

#include <pthread.h>
#include <stdlib.h> /*malloc(), */
#include <sys/types.h>  /*accept() */
#include <sys/socket.h>
#include <unistd.h> //close()
#include <signal.h> //sigaction() pthread_kill()
#include <poll.h>
#include <stdlib.h> //malloc

#include <stdio.h>
#include <string.h> //memcpy

extern Queue* LQ;
extern int SERVING_PORT;
extern char* HOST;

extern int SHUTDOWN_FLAG;

//queue mutex for accesing the queue
extern pthread_mutex_t queue_lock;

//lock when counting reposes
pthread_mutex_t response_count = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
int PENDING_RESPONSES=0;

//keep track of threads that are still crawling
pthread_mutex_t thread_count = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
int NUM_CRAWLING_THREADS=0;

/*CAUTION: serving_socket must be passed as a pointer because this
thread creator might die before all threads have initialized their
serving socket.*/
pthread_t* CreateThreadPool(int num_threads){
  pthread_cond_init(&response_update,NULL);
  pthread_t* ThreadPool = malloc(sizeof(pthread_t)*num_threads);
  for(int i=0; i<num_threads; i++){
    pthread_create(&ThreadPool[i],0,Thread,NULL);
  }
  return ThreadPool;
}

/*EVERYTHING HERE MUST BE THREAD SAFE*/
void* Thread(void* serving_socket_ptr){
  printf("thread start%d\n", (int)pthread_self());
  pthread_mutex_lock(&thread_count);
  NUM_CRAWLING_THREADS++;
  pthread_mutex_unlock(&thread_count);

  char* link;
  do{
    if(SHUTDOWN_FLAG==1)
      break;

    //get a link from queue
    pthread_mutex_lock(&queue_lock);
    link = QueueGetLink(LQ);
    /*try to get a link, if there are no pending responses then there sure wont
    be any more links. So dont bother waiting, just end.*/
    while(link == NULL && SHUTDOWN_FLAG!=1 && PENDING_RESPONSES>0){
      //if u cant get a link wait for a response
      //printf("%d waiting for a link\n", (int)pthread_self());
      pthread_cond_wait(&response_update,&queue_lock);  //pause
      //printf("woke up %d\n", (int)pthread_self());
      //link might be available now?
      link = QueueGetLink(LQ);
    }
    //this thread got a link and shall be responding
    if(link != NULL){
      pthread_mutex_lock(&response_count);
      PENDING_RESPONSES++;
      pthread_mutex_unlock(&response_count);
    }
    pthread_mutex_unlock(&queue_lock);

    //if SHUTDOWN_FLAG or there are no more threads crawling
    if(SHUTDOWN_FLAG==1 || (link == NULL && PENDING_RESPONSES==0))
      break;

    //CRAWL this link

      //make a connection to the server
      int serving_socket = SocketConnect(HOST,SERVING_PORT);
      if(serving_socket < 0){
        SHUTDOWN_FLAG=1;
        break;
      }

      //send a GET request for the page
      if(Http_GetPage(serving_socket,link) == 0){
        //read the http header from socket
        HttpResponse response = {.rescode=-1, .content_length=-1 };
        HttpGetResponse(serving_socket,&response);
        //check response
        if(response.rescode != 200){
          close(serving_socket);
          fprintf(stderr,"Server:%d on %s\n", response.rescode,link);
          //this thread got a response, and updated the queue
          pthread_mutex_lock(&response_count);
          PENDING_RESPONSES--;
          pthread_cond_signal(&response_update);
          pthread_mutex_unlock(&response_count);
          continue;
        }
        //read all data from socket
        char* pagedata = malloc(sizeof(char)*(response.content_length+1));
        NULL_Check(pagedata);
        if(read(serving_socket,pagedata,response.content_length)<0)
          exit(READ_FAIL);
        pagedata[response.content_length] = '\0';

        //save page to disk
        SavePage(link,pagedata,response.content_length);
        //parse page and insert new links to Queue (thelei mutex)
        ParseHtmlLinks(LQ,pagedata);

        free(pagedata);
      }
      close(serving_socket);

      //this thread got a response, and updated the queue
      pthread_mutex_lock(&response_count);
      PENDING_RESPONSES--;
      pthread_cond_signal(&response_update);
      pthread_mutex_unlock(&response_count);
  }while(SHUTDOWN_FLAG!=1);

  printf("thread shutdown:%d\n",(int)pthread_self());
  pthread_mutex_lock(&thread_count);
  NUM_CRAWLING_THREADS--;                 //sign off
  pthread_cond_signal(&response_update);  //chain reaction shutdown
  pthread_mutex_unlock(&thread_count);
}

void ShutdownThreads(pthread_t* ThreadPool,int num_threads){
  printf("Shutting down.\n");
  SHUTDOWN_FLAG=1;
  //wake up all threads that are waiting for links
  for(int i=0; i<num_threads; i++){
    pthread_cond_signal(&response_update);
  }
  //wait for threads to finish
  for(int i=0; i<num_threads; i++){
    if(pthread_join(ThreadPool[i],NULL)!=0)
      exit(PTHREAD_JOIN);
  }
  pthread_cond_destroy(&response_update);
}


/*Parse the html page line by line,
and if a line that represents a link is found
extract the link and add it to the queue.*/
void ParseHtmlLinks(Queue* q, char* pagedata){
  char* line;
  while((line = sgetline(pagedata)) != NULL){
    char* link = GetLineLink(line);
    if(link != NULL){
      //if this line had a link add it to the Queue
      QueueInsert(q,link);
    }
    pagedata += strlen(line);
    free(line);
  }
}
