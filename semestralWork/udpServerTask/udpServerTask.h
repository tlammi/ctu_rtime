/**
   \brief Provides UDP Server task used in semestral work


   \author Toni Lammi
 */
#ifndef UDPSERVERTASK_H
#define UDPSERVERTASK_H


#include "udpDefines.h"
#include "fifoBuffer.h"


/**
   \brief UDP Server task. Infinite loop

   This server task is used in semestral work to receive motor position measurements
   from other board. Values are then forwarded to FIFO buffer pointed by given
   parameter.

   \param fifoHandl - Handle to fifo buffer. The fifo should be initialized before this. Values read with
   UDP are written to this buffer.
*/
void udpServerTask(FifoHandl fifoHandl);

#endif
