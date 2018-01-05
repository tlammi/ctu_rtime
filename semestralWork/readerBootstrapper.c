#include "udpClientTask.h"
#include "motorReaderTask.h"



int init(){
	startMotorReader();
}


void readerBootstrap(){
	int res = init();
	
	// Bootstrap all tasks
}
