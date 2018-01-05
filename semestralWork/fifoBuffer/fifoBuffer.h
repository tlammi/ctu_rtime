/**
 * \brief FIFO interface for inter task communication
 */

#ifndef FIFOBUFFER_H
#define FIFOBUFFER_H

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
 */
FifoHandl fifo_init(FifoID id);


void fifo_push(FifoHandl handl, FIFO_DATA_TYPE push);

FIFO_DATA_TYPE fifo_pop(FifoHandl handl);

#endif