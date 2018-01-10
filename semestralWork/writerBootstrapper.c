/**
 * \brief Entry point for the board that controls a motor
 * 
 * Functions here initialize and bootstrap the system.
 * 
 * \author Toni Lammi
 */


#include "udpServerTask.h"
#include "tcpServerTask.h"
#include "motorWriterTask.h"
#include "fifoBuffer.h"
#include "priorities.h"

#include <taskLib.h>
#include <kernelLib.h>


//! Initialize system
void init(FifoHandl* motorHandl, FifoHandl* tcpHandl){
	// Handles to fifo interfaces
	*motorHandl = fifo_init(FIFO_MOTOR_WRITER);
	*tcpHandl = fifo_init(FIFO_TCP_SERVER);
}

//! Bootstrap the tasks
void writerBootstrap(){
	FifoHandl motorHandl, tcpHandl;
	init(&motorHandl, &tcpHandl);
	
	// Bootstrap all tasks
	taskSpawn("motorWriter", PRIORITY_MOTOR_WRITER, 0, 4096, (FUNCPTR) startMotorWriter, motorHandl,tcpHandl,0,0,0,0,0,0,0,0);
	taskSpawn("UDP_server", PRIORITY_UDP_SERVER, 0, 4096, (FUNCPTR) udpServerTask, motorHandl,0,0,0,0,0,0,0,0,0);
	taskSpawn("TCP_server", PRIORITY_TCP_SERVER, 0, 4096, (FUNCPTR) tcpServerTask, tcpHandl,0,0,0,0,0,0,0,0,0);
}
