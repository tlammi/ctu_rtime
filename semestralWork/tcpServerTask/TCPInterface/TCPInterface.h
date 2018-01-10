/**
   \brief TCP Server interface

   Provides an interface for HTTP server needed in semestral work
*/

#ifndef TCPINTERFACE_H
#define TCPINTERFACE_H


#include "graphInterfaceTypes.h"


#define TCP_PORT 1982
#define TCP_SERVER_ADDR "192.168.202.203"

//! Handle to interface
typedef int TCPHandle;



/**
   \brief Intializes sockets

   Initializes the OS interface and returns a handle to interface

   \ret TCPHandle - handle to interface, -1 upon failure

 */
TCPHandle TCP_init();


/**
   \brief Listens for new connections

   \param handl - Handle to interface initialized in initTCP()-function
   \ret - Handle to interface used in communicating with client (different than the one gotten from initTCP(), -1 upon failure
*/
TCPHandle TCP_listen(TCPHandle handl);

/**
   \brief Answers to client's queries
   

   \param cliHandl - handle to client interface gotten from listenTCP() function

   \return 0 upon success, -1 otherwise
*/
int TCP_answerToClient(TCPHandle cliHandl);

/**
   \brief Add data for graph

   Data is stored in ring buffer. If buffer is full the oldest value is removed and replaced with the newest value

   \param data - data-to-be added to interface
*/
void TCP_pushGraphData(GraphData data);

/**
   \brief Close TCP interface (socket)

   \param servHandl - closes the interface pointed by this handle
*/
void TCP_closeInterface(TCPHandle interfaceHandl);


#endif
