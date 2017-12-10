#ifndef GRAPHINTERFACE_H
#define GRAPHINTERFACE_H


typedef struct{
    int actPos;
    int reqPos;
    int pwmDuty;
} GraphData;

typedef enum{
    ACT_POS = 1,
    REQ_POS,
    PWM_DUTY
} GRAPH_TYPE;


void pushData(GraphData data);

int getTraceDataStr(char* actPosBuff,
		    char* reqPosBuff, char* pwmDutyBuff);



#endif
