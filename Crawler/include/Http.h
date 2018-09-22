#ifndef HTTP_H
#define HTTP_H

/******************* struct HttpResponse ****************************************/

typedef struct HttpResponse{
  int rescode;
  int content_length;
}HttpResponse;

void Response__SetResult(HttpResponse* res,char* rescodestr);

void Response__SetContLength(HttpResponse* res, char* len);

/******************* FUNCTIONS ************************************************/

//send a minimum valid http get request for a page at link, 0 on succces
int Http_GetPage(int sock,char* link);

//get a hhtp line from sock
char* HttpGetline(int sock);

//read http response lines until blank line  and update rescode and content_len
void HttpGetResponse(int sock, HttpResponse* header);

#endif
