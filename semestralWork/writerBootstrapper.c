#include "udpServerTask.h"
#include "tcpServerTask.h"
#include "motorWriterTask.h"



int init(){
	startMotorWriter();
}


void writerBootstrap(){
	int res = init();
	
	// Bootstrap all tasks
}
