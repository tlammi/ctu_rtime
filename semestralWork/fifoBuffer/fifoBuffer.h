/**
 * \brief FIFO interface for inter task communication
 * 
 * fifo_init function initilializes the interface and returns a handle to
 * initialized interface. Same handle can then be used for accessing the fifo buffers.
 * 
 * \author Toni Lammi
 */

#ifndef FIFOBUFFER_H
#define FIFOBUFFER_H

//! Has to be multiple of 3 due to TCP server
#define FIFO_BUFF_SIZE 128
#define FIFO_DATA_TYPE int

#define FIFO_UDP_CLIENT 1
#define FIFO_TCP_SERVER 2
#define FIFO_MOTOR_WRITER 3
#define FIFO_UDP_SERVER 4



typedef void* FifoHandl;
typedef int FifoID;


/**
 * \brief initialize FIFO interface

 Initializes FIFO/Ring buffer with given ID.
 The interface expects only one writer and reader.

 \param id - Number from the list in this file
             Each interface should be initialized only once
 \return Handle to FIFO buffer
 */
FifoHandl fifo_init(FifoID id);

/**
   \brief Pushes data to buffer. Blocks if full.

   \param handl - Handle to wanted buffer
   \param push - value that is pushed to buffer
*/
void fifo_push(FifoHandl handl, FIFO_DATA_TYPE push);

/**
   \brief Pop data from buffer

   \param handl - Handle to wanted buffer
   \return data read from buffer
*/
FIFO_DATA_TYPE fifo_pop(FifoHandl handl);

/**
   \brief Nonblocking push to buffer

   \param handl - Handle to buffer
   \param push - data to be pushed
   \param status - 0 if success, -1 if fail

*/
void fifo_push_nonblock(FifoHandl handl, FIFO_DATA_TYPE push, int* status);

/**
   \brief Nonblocking pop from buffer

   \param handl - Handle to wanted buffer
   \param status - 0 if successfull, -1 if failure
   \return Read value upon success, undefined otherwise
*/
FIFO_DATA_TYPE fifo_pop_nonblock(FifoHandl handl, int* status);


#endif
