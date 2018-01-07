#include "udpServerTask.h"
#include "tcpServerTask.h"
#include "motorWriterTask.h"
#include "fifoBuffer.h"

#include <taskLib.h>
#include <kernelLib.h>


void init(FifoHandl* motorHandl, FifoHandl* tcpHandl){
	*motorHandl = fifo_init(FIFO_MOTOR_WRITER);
	*tcpHandl = fifo_init(FIFO_TCP_SERVER);
}

void writerBootstrap(){
	FifoHandl motorHandl, tcpHandl;
	init(&motorHandl, &tcpHandl);
	
	// Bootstrap all tasks
	taskSpawn("UDP_server", 210, 0, 4096, (FUNCPTR) 	udpServerTask, motorHandl,0,0,0,0,0,0,0,0,0);
	taskSpawn("TCP_server", 210, 0, 4096, (FUNCPTR) tcpServerTask, tcpHandl,0,0,0,0,0,0,0,0,0);
}
