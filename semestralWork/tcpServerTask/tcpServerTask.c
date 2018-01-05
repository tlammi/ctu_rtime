#include "tcpServerTask.h"
#include "graphInterfaceTypes.h"
#include "TCPInterface.h"

#include <stdlib.h>



static void graphUpdaterTask(){

	GraphData data;
	data.actPos = rand();
	data.pwmDuty = rand();
	data.reqPos = rand();
	
	TCP_pushGraphData(data);
}

void tcpServerTask(){
	
	// TODO: spawn the graphUpdaterTask
	
	srand(0);
	TCPHandle handl = TCP_init();
	TCPHandle cliHandl;
	
	while(1){
		cliHandl = TCP_listen(handl);
		if(cliHandl == -1){
			perror("TCP_listen failed");
		}
		else{
			if(TCP_answerToClient(cliHandl) != 0){
				perror("TCP_answerToClient failed");
			}
			
		}
	}
}
