#include "SHM_interface.h"
#include "cRingBuffer.c"

#include <sys/mman.h>
#include <unistd.h>
#include <semLib.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

//! Shared memory object names
#define SHM_UDP_SERVER_NAME   "/shm_udp_server"
#define SHM_UDP_CLIENT_NAME   "/shm_udp_client"
#define SHM_TCP_SERVER_NAME   "/shm_tcp_server"
#define SHM_MOTOR_READER_NAME "/shm_motor_reader"
#define SHM_MOTOR_WRITER_NAME "/shm_motor_writer"


//! Shared mutex names (for meta data access)
#define SHM_UDP_SERVER_MUTEX   "/shm_udp_server_mutex"
#define SHM_UDP_CLIENT_MUTEX   "/shm_udp_client_mutex"
#define SHM_TCP_SERVER_MUTEX   "/shm_tcp_server_mutex"
#define SHM_MOTOR_READER_MUTEX "/shm_motor_reader_mutex"
#define SHM_MOTOR_WRITER_MUTEX "/shm_motor_writer_mutex"


//! Shared semaphore names
#define SHM_UDP_SERVER_SEM   "/shm_udp_server_sem"
#define SHM_UDP_CLIENT_SEM   "/shm_udp_client_sem"
#define SHM_TCP_SERVER_SEM   "/shm_tcp_server_sem"
#define SHM_MOTOR_READER_SEM "/shm_motor_reader_sem"
#define SHM_MOTOR_WRITER_SEM "/shm_motor_writer_sem"



#ifndef NULL
#define NULL ((void*) 0);
#endif


//! Object for data in shared memory
static struct RingBufferObject{
    SHM_BUFFER_DATATYPE data[SHM_DATA_SIZE]; //!< Actual data
    size_t rIndex; //!< Metadata: read index
    size_t dataCount; //!< Metadata: count of data in the buffer
};

//! Initialize ring buffer / shared memory data
static void ringBufferInit(struct RingBufferObject* obj){
    size_t i;
    for(i=0; i<SHM_DATA_SIZE; i++){
	obj->data[i] = 0;
    }
    obj->rIndex = 0;
    obj->dataCount = 0;
}

//! Push data to shared memory ring buffer
static void ringBufferPush(struct RingBufferObject* obj,
			   size_t startIndex,
			   SHM_BUFFER_DATATYPE* push,
			   size_t pushCount ){

    size_t i = 0;
    size_t wIndex = startIndex;

    while(i < pushCount){
	obj->data[wIndex] = push[i];
	i++;
	wIndex = (wIndex + 1)%SHM_DATA_SIZE;
    }
}

//! Pop data from shared memory ring buffer
static void ringBufferPop(struct RingBufferObject* obj,
			  size_t startIndex,
			  SHM_BUFFER_DATATYPE* pop,
			  size_t popCount){
    size_t i = 0;
    size_t rIndex = startIndex;

    while(i<popCount){
	pop[i] = obj->data[rIndex];
	i++;
	rIndex = (rIndex + 1)%SHM_DATA_SIZE;
    }
}


/**
   \brief Struct for managing shared memory
*/
struct ShmObject{
    struct RingBufferObject* obj; //! < Pointer to shared memory
    char* shmName; //! < Name of the shared memory object. Stored for closing
    SEM_ID mutex; //! < Mutex for shared meta data access
    SEM_ID sem; //! < Semaphore for consumer blocking
};

//! Return shared memory object name from SHM_ID
static const char* getShmName(SHM_ID shmId){
    const char* shmName;
    switch(shmId){
    case SHM_UDP_SERVER:
	shmName = SHM_UDP_SERVER_NAME;
	break;
    case SHM_UDP_CLIENT:
	shmName = SHM_UDP_CLIENT_NAME;
	break;
    case SHM_TCP_SERVER:
	shmName = SHM_TCP_SERVER_NAME;
	break;
    case SHM_MOTOR_READER:
	shmName = SHM_MOTOR_READER_NAME;
	break;
    case SHM_MOTOR_WRITER:
	shmName = SHM_MOTOR_WRITER_NAME;
	break;
    default:
	shmName = NULL;
    }

    return shmName;
}

//! Returns shared mutex name based on SHM_ID
static const char* getSharedMutexName(SHM_ID shmId){
    const char* sMutexName;
    switch(shmId){
    case SHM_UDP_SERVER:
	sMutexName = SHM_UDP_SERVER_MUTEX;
	break;
    case SHM_UDP_CLIENT:
	sMutexName = SHM_UDP_CLIENT_MUTEX;
	break;
    case SHM_TCP_SERVER:
	sMutexName = SHM_TCP_SERVER_MUTEX;
	break;
    case SHM_MOTOR_READER:
	sMutexName = SHM_MOTOR_READER_MUTEX;
	break;
    case SHM_MOTOR_WRITER:
	sMutexName = SHM_MOTOR_WRITER_MUTEX;
	break;
    default:
	sMutexName = NULL;
    }

    return sMutexName;
}

static const char* getSharedSemName(SHM_ID shmId){
    const char* sSemName;
    switch(shmId){
    case SHM_UDP_SERVER:
	sSemName = SHM_UDP_SERVER_SEM;
	break;
    case SHM_UDP_CLIENT:
	sSemName = SHM_UDP_CLIENT_SEM;
	break;
    case SHM_TCP_SERVER:
	sSemName = SHM_TCP_SERVER_SEM;
	break;
    case SHM_MOTOR_READER:
	sSemName = SHM_MOTOR_READER_SEM;
	break;
    case SHM_MOTOR_WRITER:
	sSemName = SHM_MOTOR_WRITER_SEM;
	break;
    default:
	sSemName = NULL;
    }

    return sSemName;
}



//! Shared memory objects are stored here
ShmObject gSHMObjects[5];


shmHandl SHM_init(SHM_ID shmId){
    int fd;
    int isShmInitNeeded = 0;
    printf("Beginning of init\n");

    const char* sMutexName = getSharedMutexName(shmId);
    if(sMutexName == NULL){
	perror("getSharedMutexName");
	goto error;
    }
    const char* sSemName = getSharedSemName(shmId);
    if(sSemName == NULL){
	perror("getSharedSemName");
	goto error;
    }
    const char* shmName = getShmName(shmId);
    if(shmName == NULL){
	perror("getShmName");
	goto error;
    }

    printf("shm name is: %s\n",shmName);

    gSHMObjects[shmId-1].mutex = semOpen(sMutexName,
					 SEM_TYPE_MUTEX,
					 SEM_FULL,
					 SEM_Q_FIFO,
					 OM_CREATE,
					 NULL);

    gSHMObjects[shmId-1].sem = semOpen(sSemName,
				       SEM_TYPE_SEMAPHORE,
				       SEM_EMPTY,
				       SEM_Q_FIFO,
				       OM_CREATE,
				       NULL);
    
    // This is for closing shared memory
    gSHMObjects[shmId-1].shmName = shmName;
    
    fd = shm_open(shmName, O_RDWR | O_CREAT | O_EXCL, S_IRUSR|S_IWUSR);
    // Success, and clear needed
    if(fd >= 0){
	isShmInitNeeded = 1;
	printf("init is needed\n");
    }
    
    // Shared memory object already exists
    else if(errno == EEXIST){
	fd = shm_open(shmName, O_RDWR | O_CREAT, S_IRUSR|S_IWUSR);
	if(fd<0){
	    perror("shm_open");
	    goto error;
	}
    }
    else{
	perror("shm_open");
	goto error;
    }

    
    gSHMObjects[shmId-1].obj =
	(struct RingBufferObject*)mmap(0,
				       sizeof(struct RingBufferObject),
				       PROT_READ | PROT_WRITE,
				       MAP_SHARED, fd, 0);
    printf("address of the memory: %lx\n",
	   (long int) gSHMObjects[shmId-1]);

    if(gSHMObjects[shmId-1].obj == (struct RingBufferObject*)MAP_FAILED){
	perror("mmap");
	goto error;
    }

    // Shared memory was not yet initialized so clear data
    if(isShmInitNeeded){
	ringBufferInit(gSHMObjects[shmId-1].obj);
    }

    close(fd);

    printf("data0: %d\n", gSHMObjects[shmId-1]->data[0]);
    printf("wIndex: %lu\n",gSHMObjects[shmId-1]->wIndex);

    

    return (shmHandl) gSHMObjects[shmId-1];
error:
    return NULL;
}


size_t SHM_push(shmHandl handl, int* data, size_t count){

    struct ShmObject* ptr = (struct ShmObject*) handl;

    /*
      Algorithm:
      1: get mutex
      2: calculate start index
      3: calculate free space
      4: Update new values to shared memory
      5: free mutex
      6: write
      7: if sem was taken, give.
    */

    semTake(ptr->mutex, WAIT_FOREVER);
    // Calculations
    size_t startIndex = (ptr->obj->rIndex + ptr->obj->dataCount)%SHM_DATA_SIZE;
    size_t freeSpace = SHM_DATA_SIZE - ptr->obj->dataCount;
    // write count is smaller of the pair <freeSpace> and <count>
    size_t writeCount = freeSpace < count ? freeSpace : count;

    // meta data update
    ptr->obj->dataCount += writeCount;
    
    semGive(ptr->mutex);

    ringBufferPush(ptr->obj, data, writeCount);

    // TODO: add semaphore check and give

    return writeCount;
}


int SHM_pop(shmHandl handl, int* buffer, int maxBytes){
    struct shmObject* ptr = (struct shmObject*) handl;

    /*
      Alogrithm:
      1: take mutex
      2: calculate important data from meta data
      3: update meta data
      4: free mutex
      5: if buffer is to be read empty, take semaphore
      6: read data
     */

    semTake(ptr->mutex, WAIT_FOREVER);
    // Read meta data
    size_t startIndex = ptr->obj->rIndex;
    size_t readCount = ptr->obj->dataCount < maxBytes
	? ptr->obj->dataCount : maxBytes;

    // Update meta data
    ptr->obj->rIndex = (ptr->obj->rIndex + readCount) % SHM_DATA_SIZE;
    ptr->obj->dataCount -= readCount;
    
    semGive(ptr->mutex);

    // Read from shared memory
    ringBufferPop(ptr->obj, startIndex, buffer, readCount);

    return readCount;
    
}


void SHM_clean(shmHandl handl){
    struct ShmObject* ptr = (struct ShmObject*)handl;
    shm_unlink(ptr->shmName);
}
