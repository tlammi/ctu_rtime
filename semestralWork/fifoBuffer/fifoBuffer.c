#include "fifoBuffer.h"

#include <stdlib.h>
#include <semLib.h>

/**
 * \brief RingBuffer type used by interface
 * 
 */
struct RingBuffer{
	FIFO_DATA_TYPE data[FIFO_BUFF_SIZE]; //!< Array for storing data
	size_t rIndex; //!< Read index in buffer
	size_t wIndex; //!< Write index in buffer
	SEM_ID dSem; //!< Data in buffer
	SEM_ID fSem; //!< Free space in buffer
};

//! Fifo buffers
struct RingBuffer gRingBuffers[4];

FifoHandl fifo_init(FifoID id){
	if(id < 1 || id > 4){
		printf("Invalid fifo ID\n");
		goto error;
	}
	// Pointer to correct buffer
	struct RingBuffer* ptr = &gRingBuffers[id-1];
	
	size_t i;
	
	for(i=0; i<FIFO_BUFF_SIZE; i++){
		ptr->data[i] = 0;
	}
	
	ptr->rIndex = 0;
	ptr->wIndex = 0;
	
	ptr->dSem = semCCreate(SEM_Q_FIFO, 0);
	ptr->fSem = semCCreate(SEM_Q_FIFO, FIFO_BUFF_SIZE);
	
	return (FifoHandl) ptr;
	
error:
	return NULL;
}


void fifo_push(FifoHandl handl, FIFO_DATA_TYPE push){
	
	struct RingBuffer* ptr = (struct RingBuffer*) handl;
	
	// Decrement the free space by one
	semTake(ptr->fSem, WAIT_FOREVER);
	
	ptr->data[ptr->wIndex] = push;
	ptr->wIndex = (ptr->wIndex + 1)% FIFO_BUFF_SIZE;
	// Increment data count by one
	semGive(ptr->dSem);
	
	
}

FIFO_DATA_TYPE fifo_pop(FifoHandl handl){
	struct RingBuffer* ptr = (struct RingBuffer*) handl;
	
	// Decrement data count by one
	semTake(ptr->dSem, WAIT_FOREVER);
	FIFO_DATA_TYPE retVal = ptr->data[ptr->rIndex];
	ptr->rIndex = (ptr->rIndex + 1)%FIFO_BUFF_SIZE;
	// Increment free space by one
	semGive(ptr->fSem);
	
	return retVal;
	
	
}

void fifo_push_nonblock(FifoHandl handl, FIFO_DATA_TYPE push, int* status){
	struct RingBuffer* ptr = (struct RingBuffer*) handl;

	if(status != NULL){
		*status = -1;
	}
	// Decrement the free space by one
	if(semTake(ptr->fSem, 0) == OK){

		ptr->data[ptr->wIndex] = push;
		ptr->wIndex = (ptr->wIndex + 1)% FIFO_BUFF_SIZE;
		// Increment data count by one
		semGive(ptr->dSem);
		if(status != NULL){
			*status = 0;
		}
	}
}

FIFO_DATA_TYPE fifo_pop_nonblock(FifoHandl handl, int* status){
	struct RingBuffer* ptr = (struct RingBuffer*) handl;

	FIFO_DATA_TYPE retVal = 0;
	if(status != NULL){
		*status = -1;
	}

	// Execute only if data left in buffer
	if(semTake(ptr->dSem, 0) == OK){
		retVal  = ptr->data[ptr->rIndex];
		ptr->rIndex = (ptr->rIndex + 1)%FIFO_BUFF_SIZE;
		if(status != NULL){
			*status = 0;
		}
		semGive(ptr->fSem);
	}

    return retVal;
}
