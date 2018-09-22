#ifndef ARGUMENTS_H
#define ARGUMENTS_H

char* PROGNAME;
int SERVING_PORT;
int COMMAND_PORT;
int NUM_THREADS;
char* SAVE_DIR;
char* HOST;
char* STARTING_URL;

void ParseArguments(int argc, char* argv[]);


#endif
