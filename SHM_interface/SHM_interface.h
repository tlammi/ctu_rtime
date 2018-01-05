/**
   \brief Provides shared memory interface
   
   Provides a shared memory interface. The interface is thread safe
   only when there is one reader and one writer.
   

   \author Toni Lammi
*/
#ifndef SHM_INTERFACE_H
#define SHM_INTERFACE_H

#include <stdint.h>
#include <stdio.h>

#include "SHM_defines.h"

#define SHM_DATA_SIZE 128
#define SHM_BUFFER_DATATYPE int

//! Handle for using the interface
typedef void* shmHandl;

/**
\brief Initialize shared memory with given ID

Initializes a ring buffer in shared memory for given ID. Allowed ID's are listed in SHM_defines.h

\param shmId - ID for shared memory object
\return shmHandl - handle to initialized shared memory object
*/
shmHandl SHM_init(SHM_ID shmId);

/**
\brief Push data into back of the buffer

Pushes data into back of the ring buffer

\param handl - handle to shared memory object (returned by SHM_init)
\param data - void pointer to write buffer
\param count - how much data is tried to be written

\return number of written bytes. If returned value is smaller than count-parameter, values counting from the beginning of data-array were written.
*/
size_t SHM_push(shmHandl handl, SHM_BUFFER_DATATYPE* data, size_t count);


/**
\brief read data from buffer and consume the data

Reads data from ring buffer and increments the read index maxBytes or until all data in buffer is consumed

\param handl - handle to shared memory object
\param buffer - read buffer
\param maxBytes - maximum count of bytes that are tried to be read from the shared memory

\return - Number of read bytes on success, otherwise negative value
*/
int SHM_pop(shmHandl handl, SHM_BUFFER_DATATYPE* buffer, size_t maxBytes);


/**
\brief Destructor for the shared memory interface

Removes the shared memory object referenced by the given shmHandl
*/
void SHM_clean(shmHandl handl);
#endif
