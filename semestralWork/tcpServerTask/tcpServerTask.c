#include "tcpServerTask.h"
#include "graphInterfaceTypes.h"
#include "TCPInterface.h"

#include <stdlib.h>
#include <taskLib.h>
#include <kernelLib.h>


static void graphUpdaterTask(){
	while(1){
		GraphData data;
		data.actPos = rand();
		data.pwmDuty = rand();
		data.reqPos = rand();
		printf("%d\n",data.actPos);
		TCP_pushGraphData(data);
		taskDelay(500);
	}
}

void tcpServerTask(){
	
	// TODO: spawn the graphUpdaterTask
	
	srand(0);
	

	taskSpawn("graphUpdater", 210, 0, 4096, (FUNCPTR) 	graphUpdaterTask, 0,0,0,0,0,0,0,0,0,0);
	
	TCPHandle handl = TCP_init();
	while(handl == -1){
		handl = TCP_init();
		taskDelay(500);
	}
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
