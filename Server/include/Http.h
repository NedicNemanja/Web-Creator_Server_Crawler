#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>  //FILE*

#include "ErrorCodes.h"

/******************* struct HttpGET *******************************************/
typedef struct HttpGET{
  char* url;
}HttpGET;

void FreeHttpGET(HttpGET* get);

void GET_SetUrl(HttpGET* get, char* url_str);

/******************* FUNCTIONS ************************************************/

#define READ_SIZE 64

extern pthread_mutex_t counter_lock;

/*Read a GET Request from a socket
and set the HttpGET.url member.
If the read was succesfull 0 is teturned.
*/
int HttpGetRequest(int sock,HttpGET* Get);

void HttpAnswer(int socket, char* answer);

void HttpAnswer403(int socket);
void HttpAnswer404(int socket);
void HttpAnswer500(int socket);

/***************** utility ***************************************************/

//1 if request ends with \r\n\r\n
int HttpRequestEnded(char* buffer, int bsize);


//Load the whole file in a string and return it
char* LoadPage(char* url);
FILE* GetPageFp(char* url); //LoadPage uses this one to get the FILE pointer

//Get local time in http format to the buffer
char* GetHttpTime();

#endif
