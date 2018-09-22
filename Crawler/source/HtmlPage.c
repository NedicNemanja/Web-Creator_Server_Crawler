#include "HtmlPage.h"
#include "ErrorCodes.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h> //mkdir
#include <sys/types.h>  //mkdir
#include <pthread.h>  //couter_lock mutex

extern char* SAVE_DIR;
extern unsigned long NUM_PAGES;//succesfull num of pages answered
extern unsigned long NUM_BYTES; //sum of page legths

//count STATS
pthread_mutex_t count_lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

void SavePage(char* pagelink,char* pagedata, int pagesize){

  int savedir_size = strlen(SAVE_DIR);
  int link_size = strlen(pagelink);
  int dest_size = savedir_size+link_size;
  char* destinationpath = malloc(sizeof(char)*(dest_size+1));
  destinationpath[dest_size] = '\0';

  //make directory for site
  char buffer[100];
  //ignore until 1st /, then read to buffer until /
  sscanf(pagelink, "/%[^/]", buffer);
  strcpy(destinationpath,SAVE_DIR);
  strcat(destinationpath,"/");
  strcat(destinationpath,buffer);
  strcat(destinationpath,"/");
  mkdir(destinationpath,0664);

  destinationpath[dest_size] = '\0';
  strcpy(destinationpath,SAVE_DIR);
  strcat(destinationpath,pagelink);

  //create file
  FILE* fp = fopen(destinationpath,"w");
  if(fp == NULL)
    exit(CANT_FOPEN_FILE);
  fprintf(fp,"%s",pagedata);
  fclose(fp);
  free(destinationpath);

  pthread_mutex_lock(&count_lock);
  NUM_PAGES++;
  NUM_BYTES += pagesize;
  pthread_mutex_unlock(&count_lock);
}
