#include <stdio.h>
#include <string.h> //strcmp
#include <stdlib.h> //atoi
#include "Arguments.h"
#include "ErrorCodes.h"

void ParseArguments(int argc, char* argv[]){
  PROGNAME = argv[0];

  int arg_index=1;
  int SP_flag=0, CP_flag=0, NT_flag=0, SD_flag=0, HOST_flag=0;

  //scan all arguements to find (-p,-c,-t,-d) except the last one
  while(arg_index < argc-1){
    //make sure there is an argument to check
    if(argv[arg_index] == NULL)
      break;

    if(strcmp(argv[arg_index],"-p")==0 && SP_flag==0){
      SERVING_PORT = atoi(argv[++arg_index]);
      SP_flag = 1;
      arg_index++;
      continue;
    }
    if(strcmp(argv[arg_index],"-c")==0 && CP_flag==0){
      COMMAND_PORT = atoi(argv[++arg_index]);
      arg_index++;
      CP_flag = 1;
      continue;
    }
    if(strcmp(argv[arg_index],"-t")==0 && NT_flag==0){
      NUM_THREADS = atoi(argv[++arg_index]);
      NT_flag = 1;
      arg_index++;
      continue;
    }
    if(strcmp(argv[arg_index],"-d")==0 && SD_flag==0){
      SAVE_DIR = argv[++arg_index];
      SD_flag = 1;
      arg_index++;
      continue;
    }
    if(strcmp(argv[arg_index],"-h")==0 && HOST_flag==0){
      HOST = argv[++arg_index];
      HOST_flag = 1;
      arg_index++;
      continue;
    }
    fprintf(stderr, "Unknown argument: %s\n", argv[arg_index]);
    exit(UNKNOWN_CMDARGUMENT);
  }

  //get last argument
  STARTING_URL = argv[arg_index];
  if(STARTING_URL == NULL){
    fprintf(stderr, "Specify a starting url as the last argument.\n");
    exit(NO_STARTING_URL);
  }

  //check that you got all the arguements
  if(SP_flag == 0){
    fprintf(stderr, "Specify a serving port using \"-p port_num\".\n");
    exit(NO_SERVING_PORT);
  }
  if(CP_flag == 0){
    fprintf(stderr, "Specify a command port using \"-c port_num\".\n");
    exit(NO_COMMAND_PORT);
  }
  if(NT_flag == 0){
    fprintf(stderr, "Specify the number of threads using \"-t num_threads\".\n");
    exit(NO_NUM_THREADS);
  }
  if(SD_flag == 0){
    fprintf(stderr, "Specify a root directory using \"-d dir_path\".\n");
    exit(NO_SAVE_DIR);
  }
  if(HOST_flag == 0){
    fprintf(stderr, "Specify a host using \"-h host_or_ip\".\n");
    exit(NO_HOST);
  }
}
