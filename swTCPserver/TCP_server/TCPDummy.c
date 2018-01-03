
#include "TCPInterface.h"

int main(){

    TCPHandle myHandl = initTCP();
    TCPHandle cliHandl = listenTCP(myHandl);

    int result = readAndAnswer(cliHandl);
}
