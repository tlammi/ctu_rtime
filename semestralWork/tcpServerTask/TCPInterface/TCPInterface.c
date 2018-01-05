#include "TCPInterface.h"
#include "graphInterface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inetLib.h>
#include <sockLib.h>


// Max num of concurrent HTTP connections
#define MAX_CONNECTIONS 5


// Some magic words sent via HTTP
#define HTTP_HEADER "HTTP/1.0 200 OK\r\n" \
    "Content-Type: text/html; charset=UTF-8\r\n\r\n"

// Notice CDN URL for plotting
#define HTML_HEAD "<head>" \
    "<script src=\"https://cdn.plot.ly/plotly-latest.min.js\">" \
    "</script>""</head>"

#define GRAPH_WIDTH_PX "1000"
#define GRAPH_HEIGHT_PX "500"
// Place holder for the plot
#define HTML_BODY "<body onload"				\
    "=\"setInterval(function(){location.reload()}, 5000);\">"	\
    "<div id=\"tester\" style=\"width:" GRAPH_WIDTH_PX		\
    "px;height:"GRAPH_HEIGHT_PX"px;\"></div>"			\
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
    "title: 'System Information',"			\
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

//! Full HTML string format
const char* HTML_FORMAT = HTTP_HEADER
    "<!DOCTYPE html>""<html>"
    HTML_HEAD
    HTML_BODY
    HTML_SCRIPT_FORMAT
    "</html>\r\n\r\n";



TCPHandle TCP_init(){
    // Socket file
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("sockfd was: %d\n",sockfd);
    int portNo = TCP_PORT;

    struct sockaddr_in myAddr;
    bzero((char*)&myAddr, sizeof(myAddr));

    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(portNo);

    char *bind_address = TCP_SERVER_ADDR;
    
    // For some reason this call fails but the server still works
    if (inet_aton(bind_address, &myAddr.sin_addr) == 0) {
    	perror("invalid bind address\n");
    	//goto error;
    }

    int status = bind(sockfd, (struct sockaddr*)&myAddr,
		      sizeof(myAddr));
    if(status < 0){
	perror("bind");
	goto error;
    }

    return sockfd;


error:
    close(sockfd);
    return -1;
}


TCPHandle TCP_listen(TCPHandle initialiHandl){
	printf("beginning of TCP_listen\n");
    listen(initialiHandl, MAX_CONNECTIONS);
    struct sockaddr_in cliAddr;
    int cliLen = sizeof(cliAddr);
    int cliSockfd = accept((int)initialiHandl,
			   (struct sockaddr*) &cliAddr, &cliLen);

    if(cliSockfd < 0){
	perror("accept");
	goto error;
    }

    return cliSockfd;

error:
    return -1;
}

char gBuffer[256];
int TCP_answerToClient(TCPHandle cliHandl){

    int i=0;

    char actPosBuff[100]; // Actual postion buffer
    char reqPosBuff[100]; // Requested position buffer
    char pwmBuff[100];    // PWM duty cycle buffer

    // Import data from graphInterface
    getGraphDataStr(actPosBuff, reqPosBuff, pwmBuff);

    // Message is stored in this
    char sendBuffer[1024*10];

    // Construct message
    sprintf(sendBuffer, HTML_FORMAT, actPosBuff, reqPosBuff, pwmBuff);

    // Send the data
    int toBeSent = strlen(sendBuffer);
    int strLen = toBeSent;
    int msgSize;
    while(toBeSent > 0){
	msgSize = write(cliHandl, sendBuffer,
			strLen);
	if(msgSize >= 0){
	    toBeSent -= msgSize;
	}
	else{
	    printf("TCP error: write returned %d",msgSize);
	    return -1;
	}
    }

    return 0;
}


void TCP_pushGraphData(GraphData data){
    pushGraphData(data);
}

void TCP_closeInterface(TCPHandle interfaceHandl){
    close(interfaceHandl);
}
