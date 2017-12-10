
#include "TCPInterface.h"

#include <stdlib.h>

int main(){

    srand(0);

    TCPHandle myHandl = initTCP();
    int iterations = 0;
    TCPHandle cliHandl;
    while(iterations < 10){
	GraphData data = {
	    .actPos = rand()%10,
	    .reqPos = rand()%10,
	    .pwmDuty = rand()%10
	};

	pushGraphData(data);
	
	cliHandl = listenTCP(myHandl);
	int result = readAndAnswer(cliHandl);
	closeClientConnection(cliHandl);
	++iterations;
    }
    closeClientConnection(cliHandl);

    exit(0);
}
