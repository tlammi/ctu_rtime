
#include "fifoBuffer.h"


struct RingBuffer{
	FIFO_DATA_TYPE data[FIFO_BUFF_SIZE];
	size_t rIndex;
	size_t wIndex;
	SEM_ID dSem; //!< Data in buffer
	SEM_ID fSem; //!< Free space in buffer
};

struct RingBuffer gRingBuffers[4];

FifoHandl fifo_init(FifoID id){
	struct RingBUffer* ptr = &gRingBuffers[id-1];
	
	size_t i;
	
	for(i=0; i<FIFO_BUFF_SIZE; i++){
		ptr->data[i] = 0;
	}
	
	ptr->rIndex = 0;
	ptr->wIndex = 0;
	
	ptr->dSem = semCCreate(SEM_Q_FIFO, 0);
	ptr->fSem = semCCreate(SEM_Q_FIFO, FIFO_BUFF_SIZE);
	
	return (FifoHandl) ptr;
}


void fifo_push(FifoHandl handl, FIFO_DATA_TYPE push){
	
	struct RingBuffer* ptr = (struct RingBuffer*) handl;
	
	// Decrement the free space by one
	semTake(ptr->fSem);
	
	ptr->data[ptr->wIndex] = push;
	ptr->wIndex = (ptr->wIndex + 1)% FIFO_BUFF_SIZE;
	
	semGive(ptr->dSem);
	
	
}

FIFO_DATA_TYPE fifo_pop(FifoHandl handl){
	struct RingBuffer* ptr = (struct RingBuffer*) handl;
	
	semTake(ptr->dSem);
	FIFO_DATA_TYPE retVal = ptr->data[ptr->rIndex];
	ptr->rIndex = (ptr->rIndex + 1)%FIFO_DATA_SIZE;
	semGive(ptr->fSem);
	
	return retVal;
	
	
}
