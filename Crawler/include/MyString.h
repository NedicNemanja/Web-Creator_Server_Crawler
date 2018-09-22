#ifndef MYSTRING_H
#define MYSTRING_H

/*tokenize t but doesnt modify the string and is thread safe.
token_index specifies which token to return (0 for the 1st etc).
If you use this function it is your responsibility to free the returned string*/
char* mystrtok(char* source, const char delimeter, int token_index);

int NumDigits(unsigned long i);

//0 if its not an ip else 1
int IsIp(char* str);

//given a link pare to find its page and return it
char* ParseLink(char* link);

//getline from string (end of line: \0,\n
char* sgetline(char* str);

/*if the given string is a html link, extract the url and return it as a string
esle return NULL*/
char* GetLineLink(char* line);


#endif
