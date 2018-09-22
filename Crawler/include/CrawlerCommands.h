#ifndef SERVERCOMMANDS_H
#define SERVERCOMMANDS_H

typedef enum COMMAND {  CONN_CLOSED=-1,
                        UNKNOWN_COMMAND,
                        STATS=1,
                        SHUTDOWN=2,
                        SEARCH=3
} COMMAND;


//Server incomind commands at command_socket
void ServeCommandSocket(int command_socket);

/*Get the command STATS or SHUTDOWN from a open connection at sock.*/
COMMAND GetCommand(int sock);

//answer to sock about STATS
void AnswerStats(int sock);

//answer to sock that an unknown command was received from it
void AnswerUnknownCommand(int sock);

//If search was requested and there are still threads crawling answer no
void AnswerCrawlingInProgress(int sock);

//not implemented
void AnswerSearch(int sock);


#endif
