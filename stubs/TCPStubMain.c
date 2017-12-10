
#include "TCPInterface.h"

#include <stdlib.h>

int main(){

    srand(0);

    TCPHandle myHandl = TCP_init();
    int iterations = 0;
    TCPHandle cliHandl;
    while(1){
	GraphData data = {
	    .actPos = rand()%10,
	    .reqPos = rand()%10,
	    .pwmDuty = rand()%10
	};

	TCP_pushGraphData(data);
	
	cliHandl = TCP_listen(myHandl);
	int result = TCP_answerToClient(cliHandl);
	TCP_closeClientConnection(cliHandl);
	++iterations;
    }
    TCP_closeClientConnection(cliHandl);

    exit(0);
}
