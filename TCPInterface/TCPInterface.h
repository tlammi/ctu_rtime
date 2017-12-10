#ifndef TCPINTERFACE_H
#define TCPINTERFACE_H


#include "graphInterface.h"

typedef void* TCPHandle;




TCPHandle initTCP();

TCPHandle listenTCP(TCPHandle handl);

int readAndAnswer(TCPHandle cliHandl);


void pushGraphData(GraphData data);

void closeTCPServer(TCPHandle servHandl);

void closeClientConnection(TCPHandle cliHandl);

#endif
