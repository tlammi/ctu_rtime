#include "udpServerTask.h"

#include <inetLib.h>
#include <sockLib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <taskLib.h>

static int init(){
	// Socket descriptor
	int sockfd;
	// Socket structs
	struct sockaddr_in my_name;
	// Message buffer
	int status;

	/* Create a UDP socket */
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1){
		perror("Socket creation error");
		goto error;
	}

	/* Configure server address */
	my_name.sin_family = AF_INET;
	my_name.sin_addr.s_addr = INADDR_ANY;
	my_name.sin_port = htons(UDP_SERVER_PORT);

	status = bind(sockfd, (struct sockaddr*)&my_name, sizeof(my_name));
	if(status == -1){
		perror("bind");
		goto error;
	}

	return sockfd;

	error:
	return -1;
}

void udpServerTask(FifoHandl fifoHandl){
	
	int sockfd = init();
	
	while(sockfd == -1){
		perror("UDP server fail");
		sockfd = init();
		taskDelay(500);
	}


	struct sockaddr_in cli_name;

	int addrlen = sizeof(cli_name);
	
	int val;

	while(1){
		
		int n = recvfrom(sockfd, &val, sizeof(int), 0, (struct sockaddr*)&cli_name, &addrlen);
		if(n == -1){
			perror("Error receiving");
		}
		else{
			fifo_push(fifoHandl, val);
		}
	}


	
}
