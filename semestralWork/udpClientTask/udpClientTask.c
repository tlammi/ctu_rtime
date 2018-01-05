#include "udpClientTask.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <taskLib.h>
#include <inetLib.h>
#include <sockLib.h>


static int initSocket(){
	int sockd;
	struct sockaddr_in my_addr;
	sockd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockd == -1){
		perror("socket");
		goto error;
	}

	// Configure client address
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = INADDR_ANY;
	my_addr.sin_port = 0;


	if(bind(sockd, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1){
		perror("bind");
		goto error;
	}

	return sockd;
	error:
	return -1;
}


void udpClientTask(FifoHandl fifoHandl){
	int sockd = initSocket();
	while(sockd == -1){
		sockd = initSocket();
		taskDelay(20);
	}
	
	struct sockaddr_in srv_addr;

	srv_addr.sin_family = AF_INET;
	inet_aton(UDP_SERVER_ADDR, &srv_addr.sin_addr);
	srv_addr.sin_port = htons(UDP_SERVER_PORT);
	
	int value = 0;
	
	while(1){
		value = fifo_pop(fifoHandl);
		int n = sendto(sockd, &val, 1, 0,(struct sockaddr*)&srv_addr, sizeof(srv_addr));
	}
}
