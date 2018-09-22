#include "MyString.h"
#include "ErrorCodes.h"

#include <stdlib.h> /*malloc()*/
#include <string.h> /*memcpy*/
#include <math.h>   //ceil(),log10()
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
