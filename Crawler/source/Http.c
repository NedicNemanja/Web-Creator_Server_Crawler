#include "Http.h"
#include "MyString.h" /*mystrtok()*/
#include "ErrorCodes.h"

#include <stdlib.h> /*malloc(), realloc(), exit()*/
#include <string.h> /*strcmp() strcpy()*/
#include <unistd.h> //read()

/****************** struct HttpResponse **************************************/

void Response__SetResult(HttpResponse* res,char* rescodestr){
  res->rescode = atoi(rescodestr);
}

void Response__SetContLength(HttpResponse* res, char* len){
  res->content_length = atoi(len);
}

/**************** FUNCTIONS **************************************************/

void HttpGetResponse(int sock,HttpResponse* header){
  char* line;
  do{
    //get a line of http response header
    line = HttpGetline(sock);
    //parse line
    if(line!=NULL){
      //get 1st word
      char* word = mystrtok(line,' ',0);

      if(strcmp(word,"HTTP/1.1") == 0){
        char* rescode = mystrtok(line,' ',1);
        Response__SetResult(header,rescode);
        free(word);
        free(rescode);
        free(line);
        continue;
      }

      if(strcmp(word,"Content-Length:") == 0){
        char* len = mystrtok(line,' ',1);
        Response__SetContLength(header,len);
        free(word);
        free(len);
        free(line);
        continue;
      }
      free(word);
      free(line);
    }
  }while(line!=NULL);//while conn open +line not blank
}

char* HttpGetline(int sock){
  char c;
  char* line=NULL;
  int line_size=0,val;
  do{
    if((val=read(sock,&c,1))<0)
      exit(READ_FAIL);

    if(c != '\r' && val != 0){
      line_size++;
      line = realloc(line,sizeof(char)*(line_size+1));
      line[line_size-1] = c;
    }
  }while(c != '\r' && val!=0);

  read(sock,&c,1);  //read \n

  if(line != NULL)
    line[line_size]='\0';  //termiante string
  return line;
}

//send a minimum valid http get request for a page at link
int Http_GetPage(int sock,char* link){
  int buffer_size = 4+strlen(link)+11+22+26;
  char* buffer = malloc(sizeof(char)*(buffer_size+1));
  buffer[buffer_size] = '\0';

  strcpy(buffer,"GET ");
  strcat(buffer,link);
  strcat(buffer," HTTP/1.1\r\n"
                "Host: mycrawler:9999\r\n"
                "Connection: Keep-Alive\r\n\r\n");
  int val;
  if((val=write(sock,buffer,buffer_size)) < buffer_size){
    free(buffer);
    fprintf(stderr,"Http_GetPage write() failed. Return value %d.\n",val);
    return 1;
  }
  free(buffer);
  return 0;
}
