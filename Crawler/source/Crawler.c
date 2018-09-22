#include "Crawler.h"
#include "Arguments.h"
#include "ErrorCodes.h"
#include "Threads.h"
#include "LinkQueue.h"
#include "MyString.h" //ParseLink()
#include "Sockets.h"
#include "CrawlerCommands.h"

#include <unistd.h> //close
#include <stdlib.h>
#include <time.h>

#include <stdio.h>

int main(int argc,char* argv[]){
  ParseArguments(argc,argv);
  CLOCK_START = time(NULL);

  //create a link queue for crawling
  LQ = CreateQueue();
  QueueInsert(LQ,ParseLink(STARTING_URL));

  //create command socket
  int command_socket;
  SBL_Socket(&command_socket,COMMAND_PORT);

  //create a thread pool to serve the connections
  pthread_t* ThreadPool = CreateThreadPool(NUM_THREADS);

  ServeCommandSocket(command_socket);

  ShutdownThreads(ThreadPool,NUM_THREADS);

  free(ThreadPool);
  FreeQueue(LQ);
  close(command_socket);

  return OK;
}
