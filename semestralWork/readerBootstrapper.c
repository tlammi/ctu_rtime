/**
 * \brief Entry point for the board that reads values from a motor
 * 
 * Functions defined here initialize and bootstrap the sytem.
 * 
 * \author Toni Lammi
 */
#include "udpClientTask.h"
#include "motorReaderTask.h"
#include "fifoBuffer.h"
#include "priorities.h"

#include <stdio.h>
#include <kernelLib.h>
#include <taskLib.h>

//! Initialize system
int init(FifoHandl* udpHandl){
	// Handle to fifo interface
    *udpHandl = fifo_init(FIFO_UDP_CLIENT);
    
    return 0;
}


//! Bootstrap the tasks
void readerBootstrap(){
    FifoHandl udpHandl;
	int res = init(&udpHandl);
	if(res < 0){
	    perror("reader init");
	}
	else{
	    taskSpawn("motorReaderStarter", PRIORITY_MOTOR_WRITER, 0, 4096, (FUNCPTR) startMotorReader, udpHandl,0,0,0,0,0,0,0,0,0);
	    taskSpawn("UDP_client", PRIORITY_UDP_CLIENT, 0, 4096, (FUNCPTR) udpClientTask, udpHandl,0,0,0,0,0,0,0,0,0);

	}
}
