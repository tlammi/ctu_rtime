#include "tcpServerTask.h"
#include "graphInterfaceTypes.h"
#include "TCPInterface.h"
#include "fifoBuffer.h"
#include "priorities.h"


#include <stdlib.h>
#include <taskLib.h>
#include <kernelLib.h>


//! Read data from fifo buffer and push it to TCP interface
static void graphUpdaterTask(FifoHandl fifoHandl){
	while(1){
		
		// Read data from buffer. This buffer call is blocking.
		int actVal, reqVal, pwmVal;
		actVal = fifo_pop(fifoHandl);
		reqVal = fifo_pop(fifoHandl);
		pwmVal = fifo_pop(fifoHandl);
		
		GraphData data;
		data.actPos = actVal;
		data.pwmDuty = pwmVal;
		data.reqPos = reqVal;
		
		// Push data to TCP server interface
		TCP_pushGraphData(data);
	}
}

void tcpServerTask(FifoHandl fifoHandl){
	
	// Spawn a task for reading fifo buffer with data coming from motor writer.
	// Priority is higher than that of server tasks
	//taskSpawn("graphUpdater", PRIORITY_TCP_SERVER-1, 0, 4096, (FUNCPTR) graphUpdaterTask, fifoHandl,0,0,0,0,0,0,0,0,0);
	
	// Initialize TCP server socket
	TCPHandle handl = TCP_init();
	while(handl == -1){
		handl = TCP_init();
		taskDelay(500);
	}
	
	// Listen to TCP queries and answer to them
	TCPHandle cliHandl;
	
	while(1){
		// Get a new connection
		cliHandl = TCP_listen(handl);
		if(cliHandl == -1){
			perror("TCP_listen failed");
		}
		else{
			// Answer to client
			if(TCP_answerToClient(cliHandl) != 0){
				perror("TCP_answerToClient failed");
			}
			
		}
		// Close connection to client
		TCP_closeInterface(cliHandl);
	}
}
