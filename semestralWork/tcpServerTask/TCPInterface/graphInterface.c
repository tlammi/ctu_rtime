#include "graphInterface.h"


#include <stdio.h>
#include <string.h>

// Buffer size
#define RING_BUFFER_SIZE 9

// Ring buffer for containing data
static GraphData gData[RING_BUFFER_SIZE];
// Write index
static unsigned gWIndex;

void pushGraphData(GraphData data){
    gData[gWIndex] = data;
    gWIndex = (gWIndex+1)%RING_BUFFER_SIZE;
}

int getGraphDataStr(char* actPosBuff, char* reqPosBuff, char* pwmDutyBuff){

	/*
    strcpy(actPosBuff, "[");
    strcpy(reqPosBuff, "[");
    strcpy(pwmDutyBuff, "[");
    int actStrIndex = 1;
    int reqStrIndex = 1;
    int pwmStrIndex = 1;
    */

    int actStrIndex = 0;
    int reqStrIndex = 0;
    int pwmStrIndex = 0;
	
    int i = gWIndex;
    
    for(;(i+1) % RING_BUFFER_SIZE != gWIndex; i = (i + 1) % RING_BUFFER_SIZE){
	
    	actStrIndex +=
    			sprintf(&actPosBuff[actStrIndex], "%u,",gData[i].actPos);
    	reqStrIndex +=
    			sprintf(&reqPosBuff[reqStrIndex], "%u,",gData[i].reqPos);
    	pwmStrIndex +=
    			sprintf(&pwmDutyBuff[pwmStrIndex], "%u,",gData[i].pwmDuty);
    }

    // Update all string indexes
    actStrIndex +=
	sprintf(&actPosBuff[actStrIndex], "%u",gData[i].actPos);
    reqStrIndex +=
	sprintf(&reqPosBuff[reqStrIndex], "%u",gData[i].reqPos);
    pwmStrIndex +=
	sprintf(&pwmDutyBuff[pwmStrIndex], "%u",gData[i].pwmDuty);


    return 0;
}

