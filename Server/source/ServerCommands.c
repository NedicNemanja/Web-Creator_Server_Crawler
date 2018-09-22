#include "ServerCommands.h"
#include "ErrorCodes.h"
#include "MyString.h"
#include "SharedGlobals.h"

#include <stdio.h>
#include <sys/types.h>  /*accept()*/
#include <sys/socket.h>
#include <stdlib.h>     //exit,malloc
#include <unistd.h>     //read() close()
#include <string.h>     //strcmp()
#include <time.h>       //clock end

int SHUTDOWN_FLAG=0;
unsigned long NUM_PAGES=0;//succesfull num of pages answered on get request
unsigned long NUM_BYTES=0; //sum of page legths

void ServeCommandSocket(int command_socket){
    int newsock;
    printf("Server listening for commands at command port.\n");
    //block until a connection is present
    if( (newsock=accept(command_socket,NULL,NULL)) < 0)
      exit(SOCKET_ACCEPT);

    COMMAND cmd;
    do{
      //get command (STATS or SHUTDOWN)
      switch( (cmd=GetCommand(newsock)) ){
        case STATS:
          AnswerStats(newsock);
          break;
        case UNKNOWN_COMMAND:
          AnswerUnknownCommand(newsock);
          break;
        case CONN_CLOSED:
          printf("Connection at command socket closed by foreign host."
                  " Waiting for a new one.\n");
          //try to get a new connection
          if( (newsock=accept(command_socket,NULL,NULL)) < 0)
            exit(SOCKET_ACCEPT);
          break;
      }
    }while(cmd != SHUTDOWN);
    printf("Shutting down.\n");
    SHUTDOWN_FLAG=1;
    close(newsock);
}

COMMAND GetCommand(int sock){
  char* command = NULL;
  int csize=0;

  //read a word from socket
  char letter, val;
  while((val=read(sock,&letter,1)) == 1){
    if(letter=='\r' || letter=='\n' || letter==' ') //end fo word
      break;
    csize++;
    command = realloc(command,sizeof(char)*(csize+1));
    NULL_Check(command);
    command[csize-1] = letter;
    command[csize] = '\0';
  }

  //read junk after the word until end of line
  while(letter!='\n' && val!=0){
    val=read(sock,&letter,1);
  }

  if(val == 0){
    if(command!=NULL)
      free(command);
    return CONN_CLOSED;
  }

  if(strcmp(command,"STATS")==0){
    free(command);
    return STATS;
  }
  if(strcmp(command,"SHUTDOWN")==0){
    free(command);
    return SHUTDOWN;
  }
  free(command);
  return UNKNOWN_COMMAND;
}


extern time_t CLOCK_START;

void AnswerStats(int sock){
  clock_t clock_end = time(NULL);
  time_t runtime = (clock_end-CLOCK_START);

  int buffer_size = 47 + NumDigits(NUM_PAGES) + NumDigits(NUM_BYTES)
                       + NumDigits(runtime);
  char* buffer = malloc(sizeof(char)*(buffer_size+1));

  sprintf(buffer, "Num of pages:%lu\n"
                  "Num of bytes:%lu\n"
                  "Server up for: %lusec\n",
                  NUM_PAGES, NUM_BYTES, runtime);
  write(sock,buffer,buffer_size);
  free(buffer);
}

void AnswerUnknownCommand(int sock){
  write(sock,"Unknown command. Use STATS or SHUTDOWN.\n", 40);
}
