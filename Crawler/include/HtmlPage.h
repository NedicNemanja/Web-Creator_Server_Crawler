#ifndef HTMLPAGE_H
#define HTMLPAGE_H
#include <pthread.h>

//count STATS
pthread_mutex_t count_lock;

void SavePage(char* pagelink,char* pagedata, int pagesize);

#endif
