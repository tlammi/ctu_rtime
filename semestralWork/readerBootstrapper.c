#include "udpClientTask.h"
#include "motorReaderTask.h"

#include <stdio.h>
#include <kernelLib.h>
#include <taskLib.h>

int init(FifoHandl* udpHandl){
    *udpHandl = fifo_init(FIFO_UDP_CLIENT);
}


void readerBootstrap(){
    FifoHandl udpHandl;
	int res = init(&udpHandl);
	if(res < 0){
	    perror("reader init");
	}
	else{
	    startMotorReader();
	    taskSpawn("UDP_client", 210, 0, 4096, (FUNCPTR) udpClientTask, udpHandl,0,0,0,0,0,0,0,0,0);
	    
	}
}
