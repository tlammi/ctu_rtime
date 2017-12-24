#ifndef TCPINTERFACE_H
#define TCPINTERFACE_H


typedef void* TCPHandle;




TCPHandle initTCP();

TCPHandle listenTCP(TCPHandle handl);

int readAndAnswer(TCPHandle cliHandl);


#endif
