#ifndef UDPCLIENTTASK_H
#define UDPCLIENTTASK_H

#include "fifoBuffer.h"
#include "udpDefines.h"

/**
 * \brief Task for UDP client
 * 
 * This task is used for pushing values read from one motor to another board
 * 
 * \param fifoHandl - Handle to fifo buffer between motor driver and udp client
*/
void udpClientTask(FifoHandl fifoHandl);

#endif
