#include "Http.h"
#include "MyString.h" /*mystrtok()*/
#include "SharedGlobals.h"

#include <stdlib.h> /*malloc(), realloc(), exit()*/
#include <string.h> /*strcmp() strcpy()*/
#include <stdio.h>  //fseek,ftell,fread...fopen,fclose...
#include <time.h>   //used by GetHttpTime()
#include <errno.h>  //check errno after fopen
#include <unistd.h> //read()
#include <pthread.h>  //pthread_mutex_lock/unlock

/****************** struct HttpGET *******************************************/

void FreeHttpGET(HttpGET* get){
  if(get->url != NULL)
    free(get->url);
}

void GET_SetUrl(HttpGET* get, char* url_str){
  get->url = url_str;
}


/**************** FUNCTIONS **************************************************/

/*Read a GET Request from a socket
and set the HttpGET.url member
Reads until the blank line is read.
If the read was succesfull 0 is returned
*/
int HttpGetRequest(int sock,HttpGET* Get){
  //read the GET to buffer (in chunks)
  char* buffer = NULL;
  int bsize=0, val;
  int done_flag=0;
  do{
    //read READ_SIZE bytes every time (chunk by chunk)
    buffer = realloc(buffer,bsize+READ_SIZE);
    NULL_Check(buffer);
    if( (val = read(sock,buffer+bsize,READ_SIZE)) < 0)
      exit(READ_FAIL);
    bsize += val;
  //while conn open+req not complete
  }while(val!=0 && HttpRequestEnded(buffer,bsize)!=1);

  //check for LF at the end of the buffer string (end of line)
  if(HttpRequestEnded(buffer,bsize)){
      //get 1st word
      char* word = mystrtok(buffer,' ',0);
      //if its the GET line
      if(strcmp(word,"GET") == 0 && Get->url==NULL){
        //get 2nd work, its the url
        GET_SetUrl(Get,mystrtok(buffer,' ',1));
      }
      else{
        fprintf(stderr,"http req has bad syntax\n");
        free(word);
        free(buffer);
        return 2;   //bad http GET syntax
      }
      free(word);
  }
  else{
    fprintf(stderr,"http req has not ended\n");
    free(buffer);
    return 1; //read everyting from scoket, but GET still incomplete
  }
  free(buffer);
  return 0; //all good
}

void HttpAnswer(int socket, char* answer){
  if( answer == NULL){
    if(errno == ENOENT){           //page not found
      HttpAnswer404(socket);
      return;
    }
    else if(errno == EACCES ){   //server has no permission
      HttpAnswer403(socket);
      return;
    }
    else{                       //internal server error
      HttpAnswer500(socket);
      return;
    }
  }
  //page found return it as answer
  write(socket,"HTTP/1.1 200 OK\r\n", 24);

  write(socket,"Date: ", 6);
  char* timestr = GetHttpTime();
  write(socket,timestr, 29);
  write(socket,"\r\n", 2);
  free(timestr);

  write(socket, "Server: myhttpd/1.0.0\r\n", 23);

  write(socket, "Content-Length: ", 16);
  unsigned long asize = strlen(answer);
  char* buffer = malloc(sizeof(char)*(NumDigits(asize)+1));
  sprintf(buffer,"%lu",asize);
  write(socket,buffer,NumDigits(asize));
  free(buffer);
  write(socket, "\r\n", 2);

  write(socket, "Content-Type: text/html\r\n" //25
                "Connection: Closed\r\n"      //20
                "\r\n",                       //2
                47);
  write(socket, answer, asize);
  free(answer);

  //update couters
  pthread_mutex_lock(&counter_lock);
  NUM_PAGES++;
  NUM_BYTES+=asize;
  pthread_mutex_unlock(&counter_lock);
}

void HttpAnswer403(int socket){
  printf("403 FORBIDDEN\n");
  write(socket,"HTTP/1.1 403 Forbidden\r\n", 24);

  write(socket,"Date: ", 6);
  char* timestr = GetHttpTime();
  write(socket,timestr, 29);
  write(socket,"\r\n", 2);
  free(timestr);

  write(socket, "Server: myhttpd/1.0.0\r\n"   //23
                "Content-Length: 26\r\n"      //20
                "Content-Type: text/html\r\n" //25
                "Connection: Closed\r\n"      //20
                "\r\n"                        //2
                "<html>403 Forbidden</html>", //26
                116);
}

void HttpAnswer404(int socket){
  printf("404 Not Found\n");
  write(socket,"HTTP/1.1 404 Not Found\r\n",24);

  write(socket,"Date: ", 6);
  char* timestr = GetHttpTime();
  write(socket,timestr, 29);
  write(socket,"\r\n", 2);
  free(timestr);

  write(socket, "Server: myhttpd/1.0.0\r\n"   //23
                "Content-Length: 26\r\n"      //20
                "Content-Type: text/html\r\n" //25
                "Connection: Closed\r\n"      //20
                "\r\n"                        //2
                "<html>404 Not Found</html>", //26
                116);
}

void HttpAnswer500(int socket){
  write(socket,"HTTP/1.1 500 Internal Server Error\r\n", 24);

  write(socket,"Date: ", 6);
  char* timestr = GetHttpTime();
  write(socket,timestr, 29);
  write(socket,"\r\n", 2);
  free(timestr);

  write(socket, "Server: myhttpd/1.0.0\r\n"   //23
                "Content-Length: 38\r\n"      //20
                "Content-Type: text/html\r\n" //25
                "Connection: Closed\r\n"      //20
                "\r\n"                        //2
                "<html>500 Internal Server Error</html>", //38
                128);
}

/******************** utility ************************************************/
//1 if request ends with \r\n\r\n
int HttpRequestEnded(char* buffer,int bsize){
  if(bsize<4)
    return 0;

  char* request = buffer+bsize;
  if( *(request-4) == '\r' &&
      *(request-3) == '\n' &&
      *(request-2) == '\r' &&
      *(request-1) == '\n'){
    return 1;
  }
  return 0;
}

extern char* ROOT_DIR;  //name of root_directory (contains the pages)


//Load the whole file in a string and return it
char* LoadPage(char* url){
  if(url == NULL)
    return NULL;

  FILE* fp = GetPageFp(url); /*find page on disk, open it and return fp
                            if something goes wrong return NULL and respond*/
  if(fp == NULL)
    return NULL;

  /*** this segment was inspired from:  (edit+comments are mine)
  https://stackoverflow.com/questions/174531/easiest-way-to-get-files-contents-in-c
  this will fail for files >4GB
  */
  char * buffer = 0;
  long length;

  fseek (fp, 0, SEEK_END);  //set file pos indicator to the end of the file
  length = ftell (fp);      //obtain current value of file pos indicator
  //GetPageFp:fopen() bug
  fseek (fp, 0, SEEK_SET);  //return to start
  buffer = malloc(sizeof(char)*(length+1)); //allocate space for the whole file
  NULL_Check(buffer);
  fread (buffer, 1, length, fp); //read the file to buffer
  buffer[length] = '\0';
  /*** end of segment *****************/

  if(fclose(fp) != 0)
    exit(CANT_CLOSE_FILE);

  return buffer;
}

/*open fp for file ROOT_DIR/url
if you cannot open it then return an error code*/
FILE* GetPageFp(char* url){
  if(url == NULL || strlen(url)<1)
    return NULL;

  char* page_path = malloc(sizeof(char)*(3+strlen(ROOT_DIR)+1+strlen(url)+1));
  strcpy(page_path,"../");
  strcat(page_path,ROOT_DIR);
  strcat(page_path,"/");
  strcat(page_path,url);

  //fopen bug: on some paths that dont exist this might return non null
  FILE* fp = fopen(page_path,"rb");
  free(page_path);

  return fp;
}

/*FROM:
https://stackoverflow.com/questions/7548759/generate-a-date-string-in-http-response-date-format-in-c
*/
char* GetHttpTime(){
  char* buf = malloc(sizeof(char)*30);
  NULL_Check(malloc);
  time_t now = time(0);
  struct tm tm = *gmtime(&now);
  strftime(buf, 30, "%a, %d %b %Y %H:%M:%S %Z", &tm);
  return buf;
}
