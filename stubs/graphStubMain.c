

#include "graphInterface.h"

#include <stdio.h>
#include <stdlib.h>


int main(){
    int i = 0;
    int iterations = 100;

    srand(NULL);

    GraphData datas[8];

    const int buffSize = 100;
    char actPosBuff[buffSize];
    char reqPosBuff[buffSize];
    char pwmBuff[buffSize];

    for(; i<iterations; ++i){
	GraphData data = {
	    .actPos = rand(),
	    .reqPos = rand(),
	    .pwmDuty = rand()
	};
	datas[i%8] = data;

	pushData(data);

	// Buffer is full -> test
	if(i%8 == 7){
	    int i2 = 0;
	    printf("Graph datas:\n");
	    for(; i2< 8; ++i2){
		printf("[%d, %d, %d]\n",datas[i2].actPos,
		       datas[i2].reqPos, datas[i2].pwmDuty);
	    }
	    i2 = 0;
	    printf("From interface:\n");
	    getTraceDataStr(actPosBuff, reqPosBuff, pwmBuff);

	    printf("%s\n%s\n%s\n",actPosBuff, reqPosBuff, pwmBuff);

	    char c;
	    c = getchar();
	}
	
    }
}
