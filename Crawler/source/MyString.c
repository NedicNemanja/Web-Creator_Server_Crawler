#include "MyString.h"
#include "ErrorCodes.h"

#include <stdlib.h> /*malloc()*/
#include <string.h> /*memcpy*/
#include <math.h>   //ceil(),log10()
#include <ctype.h>  //isdigit()

/*tokenize t but doesnt modify the string and is thread safe.
token_index specifies which token to return (0 for the 1st etc).
If you use this function it is your responsibility to free the returned string*/
char* mystrtok(char* source, const char delimeter, int token_index){
  int index=0;  //index for parsing the source
  //find where the token starts
  for(int i=0; i<token_index; i++){
    //find delimeter
    while(source[index] != delimeter && source[index] != '\0' )
      index++;
    //skip the delimeter
    index++;
  }
  int start=index;
  //find the token ends
  int size=0;
  while(source[index] != delimeter && source[index] != '\0' ){
    size++;
    index++;
  }
  //create token
  char* substring = malloc(sizeof(char)*(size+1));
  NULL_Check(substring);
  memcpy(substring,source+start,size);
  substring[size] = '\0';

  return substring;
}

int NumDigits(unsigned long i){
  if(i==0)
  return 1;
  /*do i+1 because in the case of i=10 log10(10)=1 and we need 2*/
  return (int)ceil(log10(i+1));
}

//0 if its not an ip
int IsIp(char* str){
  int str_index=0;
  while(str[str_index]!='\0'){
    //if not '.' and not a digit 0-9
    if((str[str_index]!='.') && (isdigit(str[str_index])==0))
      return 0;
    str_index++;
  }
  return 1;
}


//parses a link/url
char* ParseLink(char* link){
  char ip[100];
  int port;
  char* page=malloc(sizeof(char)*100);
  NULL_Check(page);
  //after http:// read and discard everything until /
  //read and store up to the end
  if(sscanf(link, "http://%*[^/]%s", page) != 1){
    fprintf(stderr, "Bad starting link. Use http://hostnameandport/site/page\n");
    free(page);
    exit(BAD_STARTLINK);
  }
  page = realloc(page,sizeof(char)*(strlen(page)+1));
  NULL_Check(page);

  return page;
}

//getline from string (end of line: \0,\n
char* sgetline(char* str){
  char* line = NULL;
  int line_size=0;

  while(str[line_size]!='\0'){
    line_size++;
    line = realloc(line,sizeof(char)*(line_size+1));
    NULL_Check(line);
    line[line_size-1] = str[line_size-1];
    //if you insert \n break
    if(line[line_size-1] == '\n'){
      break;
    }
  }

  if(line == NULL)
    return NULL;

  line[line_size] = '\0';
  return line;
}

/*if the given string is a html link, extract the url and return it as a string
esle return NULL*/
char* GetLineLink(char* line){
  char buffer[100];
  //read: <a href=", ignore until backslash, read till ",
  if(sscanf(line, "<a href=\"%*[^/]%[^\"]", buffer) != 0){
    int url_size = strlen(buffer);
    char* url = malloc(sizeof(char)*(url_size+1));
    NULL_Check(url);
    url[url_size] = '\0';
    strcpy(url,buffer);
    return url;
  }
  return NULL;
}
