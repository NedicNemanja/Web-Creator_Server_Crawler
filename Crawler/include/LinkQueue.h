#ifndef LINKQUEUE
#define LINKQUEUE

#include <pthread.h>

typedef struct LinkQueue {
  char** Array; //array of strings
  int size;
  int front;  //always shows where the next link should be taken from
  int rear;   //always shows there the next link shoudl be inserted to
}Queue;

Queue* LQ;

//init queue mutex for accesing the queue
pthread_mutex_t queue_lock;

//alloc and init queue
Queue* CreateQueue();

void FreeQueue(Queue* Q);

//realloc queue and insert item at the rear
void QueueInsert(Queue* Q,char* link);

//Get a link from the front of the link queue
char* QueueGetLink(Queue* Q);


/*a dumb way to find out if the link is already in the queue & avoid duplicates
1 if it is in queue 0 if its not a duplicate*/
int LinkAlreadyInQueue(Queue* Q,char* link);

#endif
