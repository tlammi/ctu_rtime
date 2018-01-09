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
	printf("sockfd: %d\n",sockfd);

	// Configure server address 
	my_name.sin_family = AF_INET;
	inet_aton(UDP_SERVER_ADDR, &my_name.sin_addr.s_addr);
	my_name.sin_port = htons(UDP_SERVER_PORT);
	printf("UDP server binding.\n");
	status = bind(sockfd, (struct sockaddr*)&my_name, sizeof(my_name));
	if(status == -1){
		perror("bind");
		goto error;
	}
	printf("successful bind\n");

	return sockfd;

	error:
	return -1;
}

void udpServerTask(FifoHandl fifoHandl){
	
	printf("before init/n");
	int sockfd = init();
	printf("after init. got: %d\n",sockfd);
	
	while(sockfd == -1){
		perror("UDP server fail");
		sockfd = init();
		taskDelay(500);
	}


	struct sockaddr_in cli_name;

	int addrlen = sizeof(cli_name);
	
	int val;

	while(1){

		int n = recvfrom(sockfd, &val, sizeof(FIFO_DATA_TYPE), 0, (struct sockaddr*)&cli_name, &addrlen);
		if(n == -1){
			perror("Error receiving");
		}
		if(n == 4){
			// Blocking push to fifo
			fifo_push(fifoHandl, val);
		}
	}
}
