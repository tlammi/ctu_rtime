#include "graphInterface.h"


#include <stdio.h>
#include <string.h>

// Buffer size, has to be multiple of 2
#define RING_BUFFER_SIZE 8

// Ring buffer for containing data
static GraphData gData[RING_BUFFER_SIZE];
// Write index
static unsigned gWIndex;

void pushGraphData(GraphData data){
    gData[gWIndex] = data;
    gWIndex = (gWIndex+1)%RING_BUFFER_SIZE;
}

int getGraphDataStr(char* actPosBuff, char* reqPosBuff, char* pwmDutyBuff){

    strcpy(actPosBuff, "[");
    strcpy(reqPosBuff, "[");
    strcpy(pwmDutyBuff, "[");
    int actStrIndex = 1;
    int reqStrIndex = 1;
    int pwmStrIndex = 1;
    
    int i = gWIndex;
    
    for(;(i+1) % RING_BUFFER_SIZE
	    != gWIndex; i = (i + 1) % RING_BUFFER_SIZE){
	
	actStrIndex +=
	    sprintf(&actPosBuff[actStrIndex], "%d,",gData[i].actPos);
	reqStrIndex +=
	    sprintf(&reqPosBuff[reqStrIndex], "%d,",gData[i].reqPos);
	pwmStrIndex +=
	    sprintf(&pwmDutyBuff[pwmStrIndex], "%d,",gData[i].pwmDuty);
    }

    actStrIndex +=
	sprintf(&actPosBuff[actStrIndex], "%d]",gData[i].actPos);
    reqStrIndex +=
	sprintf(&reqPosBuff[reqStrIndex], "%d]",gData[i].actPos);
    pwmStrIndex +=
	sprintf(&pwmDutyBuff[pwmStrIndex], "%d]",gData[i].actPos);


    return 0;
}

