#ifndef MYSTRING_H
#define MYSTRING_H

/*tokenize t but doesnt modify the string and is thread safe.
token_index specifies which token to return (0 for the 1st etc).
If you use this function it is your responsibility to free the returned string*/
char* mystrtok(char* source, const char delimeter, int token_index);

int NumDigits(unsigned long i);

#endif
