#include "LinkQueue.h"
#include "ErrorCodes.h"

#include <string.h> //strcmp
#include <stdlib.h> //NULL malloc realloc
#include <pthread.h>  //mutexes and cond

Queue* LQ=NULL;

//init queue mutex for accesing the queue
pthread_mutex_t queue_lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

Queue* CreateQueue(){
  Queue* Q = malloc(sizeof(Queue));
  NULL_Check(Q);
  //init
  Q->Array = NULL;
  Q->size = 0;
  Q->front = 0;
  Q->rear = 0;
  return Q;
}

void FreeQueue(Queue* Q){
  //free strings (not the 1st, its and cmd argument)
  for(int i=0; i<Q->size; i++){
    if(Q->Array[i] != NULL)
      free(Q->Array[i]);
  }
  //free Array
  if(Q->Array != NULL)
    free(Q->Array);
  //free queue
  free(Q);
}

void QueueInsert(Queue* Q,char* link){
  pthread_mutex_lock(&queue_lock);

  if(LinkAlreadyInQueue(Q,link)){   //avoid duplicates
    pthread_mutex_unlock(&queue_lock);
    free(link);
    return;
  }

  //increase queue
  (Q->size)++;
  Q->Array = realloc(Q->Array,sizeof(char*)*(Q->size));
  NULL_Check(Q->Array);
  //insert item to rear
  Q->Array[Q->rear] = link;
  (Q->rear)++;
  printf("QInsert:%s\n", link);
  pthread_mutex_unlock(&queue_lock);
}

//Get a link from the front of the link queue
char* QueueGetLink(Queue* Q){
  if(Q->front < Q->rear){
    return Q->Array[(Q->front)++];
  }
  else{
    //no link available
    return NULL;
  }
}

//a dumb way to find out if the link is already in the queue & avoid duplicates
int LinkAlreadyInQueue(Queue* Q,char* link){

  for(int i=0; i<Q->size; i++){
    if(strcmp(Q->Array[i],link) == 0)
      return 1;
  }
  return 0;
}
