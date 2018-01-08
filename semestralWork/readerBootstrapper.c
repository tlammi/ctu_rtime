#include "udpClientTask.h"
#include "motorReaderTask.h"
#include "fifoBuffer.h"
#include "priorities.h"

#include <stdio.h>
#include <kernelLib.h>
#include <taskLib.h>

int init(FifoHandl* udpHandl){
    *udpHandl = fifo_init(FIFO_UDP_CLIENT);
    
    return 0;
}


void readerBootstrap(){
    FifoHandl udpHandl;
	int res = init(&udpHandl);
	if(res < 0){
	    perror("reader init");
	}
	else{
	    startMotorReader();
	    taskSpawn("UDP_client", PRIORITY_UDP_CLIENT, 0, 4096, (FUNCPTR) udpClientTask, udpHandl,0,0,0,0,0,0,0,0,0);
	    
	}
}
