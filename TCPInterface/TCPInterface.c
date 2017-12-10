#include "TCPInterface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Max num of concurrent HTTP connections
#define MAX_CONNECTIONS 5

// Some magic words sent via HTTP
#define HTTP_HEADER "HTTP/1.0 200 OK\r\n" \
    "Content-Type: text/html; charset=UTF-8\r\n\r\n"

// Notice CDN URL for plotting
#define HTML_HEAD "<head>" \
    "<script src=\"https://cdn.plot.ly/plotly-latest.min.js\">" \
    "</script>""</head>"

#define GRAPH_WIDTH_PX 2000
#define GRAPH_HEIGHT_PX 1000
// Place holder for the plot
#define HTML_BODY "<body>"\
    "<div id=\"tester\" style=\"width:" GRAPH_WIDTH_PX \
    "px;height:"GRAPH_HEIGHT_PX"px;\"></div>"	       \
    "</body>"

// Plotly.plot(<place holder>, <data>, <layout>)
#define HTML_SCRIPT_FORMAT "<script>"			\
    "TESTER = document.getElementById('tester');"	\
    "var trace1 = {"					\
    "x: [-2.00, -1.75, -1.50, -1.25, -1.00,"		\
    "-0.75, -0.50, -0.25, 0.00],"			\
    "y: %s,"						\
    "name: 'Actual Motor Position',"			\
    "type: 'scatter'"					\
    "};"						\
    "var trace2 = {"					\
    "x: [-2.00, -1.75, -1.50, -1.25, -1.00,"		\
    "-0.75, -0.50, -0.25, 0.00],"			\
    "y: %s,"						\
    "name: 'Required Motor Position',"			\
    "type: 'scatter'"					\
    "};"						\
    "var trace3 = {"					\
    "x: [-2.00, -1.75, -1.50, -1.25, -1.00,"		\
    "-0.75, -0.50, -0.25, 0.00],"			\
    "y: %s,"						\
    "name: 'PWM Duty Cycle',"				\
    "type: 'scatter'"					\
    "};"						\
    "var data = [trace1, trace2, trace3];"		\
    "var layout = {"					\
    "title: 'System Information',"				\
    "xaxis: {"						\
    "title: 'x Axis',"					\
    "titlefont: {"					\
    "family: 'Courier New, monospace',"			\
    "size: 18,"						\
    "color: '#7f7f7f'"					\
    "}"							\
    "},"						\
    "yaxis: {"						\
    "title: 'y Axis',"					\
    "titlefont: {"					\
    "family: 'Courier New, monospace',"			\
    "size: 18,"						\
    "color: '#7f7f7f'"					\
    "}"							\
    "}"							\
    "};"						\
    "Plotly.plot(TESTER, data, layout)"			\
    "</script>"

const char* HTML_FORMAT = HTTP_HEADER
    "<!DOCTYPE html>""<html>"
    HTML_HEAD
    HTML_BODY
    HTML_SCRIPT_FORMAT
    "</html>\r\n\r\n";



TCPHandle initTCP(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    int portNo = 1982;

    struct sockaddr_in myAddr;
    bzero((char*)&myAddr, sizeof(myAddr));

    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(portNo);

    printf("Before bind\n");
    char *bind_address = "127.0.0.1";
    if (inet_aton(bind_address, &myAddr.sin_addr) == 0) {
	printf("invalid bind address\n");
	goto error;
    }

    int status = bind(sockfd, (struct sockaddr*)&myAddr,
		      sizeof(myAddr));
    printf("after bind\n");
    if(status < 0){
	perror("bind");
	goto error;
    }

    return (TCPHandle) sockfd;


error:
    close(sockfd);
    return (TCPHandle) (-1);
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

error:
    return (TCPHandle) (-1);
}

char gBuffer[256];
int readAndAnswer(TCPHandle cliHandl){

    int i=0;

    char actPosBuff[100];
    char reqPosBuff[100];
    char pwmBuff[100];

    getTraceDataStr(actPosBuff, reqPosBuff, pwmBuff);
    
    char sendBuffer[1024*1024];
    
    sprintf(sendBuffer, HTML_FORMAT, actPosBuff, reqPosBuff, pwmBuff);

    int msgSize = write((int) cliHandl, sendBuffer, strlen(sendBuffer));
    printf("sent: %s\n",sendBuffer);

    return 0;
}


void pushGraphData(GraphData data){
    pushData(data);
}

void closeTCPServer(TCPHandle servHandl){
    close((int) servHandl);
}


void closeClientConnection(TCPHandle cliHandl){
    close((int) cliHandl); 
}
