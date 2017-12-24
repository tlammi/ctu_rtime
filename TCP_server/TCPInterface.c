#include "TCPInterface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CONNECTIONS 5

const char* HTML_STR = "<html>"
    "<header>"
    "Hello World!"
    "</header>"
    "<body>"
    "Hello World"
    "</body>"
    "</html>\r\n\r\n";



TCPHandle initTCP(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    int portNo = 80;

    struct sockaddr_in myAddr;
    bzero((char*)&myAddr, sizeof(myAddr));

    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = INADDR_ANY;
    myAddr.sin_port = htons(portno);

    int status = bind(sockfd, (struct sockaddr*)&myAddr,
		      sizeof(myAddr));
    if(status < 0){
	perror("bind");
	goto error;
    }

    return (TCPHandle) sockfd;
}


TCPHandle listenTCP(TCPHandle initialiHandl){
    listen((int)initialiHandl, MAX_CONNECTIONS);
    struct sockaddr_in cliAddr;
    int cliLen = sizeof(cliAddr);
    int cliSockfd = accept((int)initialiHandl,
			   (struct sockaddr*) &cliAddr, &cliLen);

    if(cliSockfd < 0){
	perror("accept");
	goto error;
    }

    return (TCPHandle) cliSockfd;
}

char gBuffer[256];
int readAndAnswer(TCPHandle cliHandl){
    int msgSize = read((int) cliHandl, gBuffer, 255);
    gBuffer[msgSize] = '\0';

    printf("got: \n%s\n", gBuffer);

    msgSize = write((int) cliHandl, HTML_STR, strlen(HTML_STR));
    printf("sent: %d\n",msgSize);

    return 0;
}
