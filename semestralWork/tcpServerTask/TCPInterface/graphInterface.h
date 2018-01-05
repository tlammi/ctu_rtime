/**
   \brief Contains data for HTML graph

   Data is stored in ring buffer and oldest
   value is dropped once newer value is added
*/
#ifndef GRAPHINTERFACE_H
#define GRAPHINTERFACE_H

#include "graphInterfaceTypes.h"


/**
   \brief Pushes new data into database

   When new data is pushed into database,
   the oldest value is overwritten

   \param data - data to be added
*/
void pushGraphData(GraphData data);

/**
   \brief Get strings from database

   Returns database values used by TCP interface for producing plots
   into HTML.
   E.g. if stored actual position values are 
   1,2,3,4,5,6,7 and 8 as integers, then actPosBuff would contain string
   "[1,2,3,4,5,6,7,8]". This is easy to add into HTML string.
   
   \param actPosBuff - Actual position string buffer
   \param reqPosBuff - Required position string buffer
   \param pwmDutyBuff - PWM duty cycle string buffer
   
   \return 0
*/
int getGraphDataStr(char* actPosBuff,
		    char* reqPosBuff, char* pwmDutyBuff);



#endif
