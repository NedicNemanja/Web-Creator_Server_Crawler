#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>

#include "ErrorCodes.h"
#include "LinkQueue.h"

/*count pending responses to the crawler threads. **********IMPORTATNO**********
If a thread cant get a link and there are pending responses to any thread,
they will wait for that response because it might discover new links to crawl.
When we get the response response_update is signaled, a thread will wake up and
try to get a new link (if the response discovered a new link it will be assigned
to the thread, but the response might as well be a already crawled link in which
case it is ignored and the thread is put back to sleep).
If there are no pending responses the thread will not wait, because with the
queue empty and no repsonses incoming there is no chance of any new links.
This ends the crawling for one thread.
But this thread should also signal the response_update,
this will start a chain reaction that will shutdown all threads.*/
pthread_mutex_t response_count;
pthread_cond_t response_update;
int PENDING_RESPONSES;

//keep track of threads that are still crawling
pthread_mutex_t thread_count;
int NUM_CRAWLING_THREADS;

pthread_t* CreateThreadPool(int num_threads);

void* Thread(void* serving_socket_ptr);

/*Parse the html page line by line,
and if a line that represents a link is found
extract the link and add it to the queue.*/
void ParseHtmlLinks(Queue* q, char* pagedata);

//signal queue_not_empty cond variable so the threads shutdown and wait for them
void ShutdownThreads(pthread_t* ThreadPool,int num_threads);

//used by thread to set up a handler for when they shutdown
void SetShutDownHandler();
void shutdown_handler(int signum);
#endif
