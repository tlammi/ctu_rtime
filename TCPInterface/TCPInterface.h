/**
   \brief TCP Server interface

   Provides an interface for HTTP server needed in semestral work
*/

#ifndef TCPINTERFACE_H
#define TCPINTERFACE_H


#include "graphInterfaceTypes.h"


//! Handle to interface
typedef void* TCPHandle;



/**
   \brief Intializes sockets

   Initializes the OS interface and returns a handle to interface

   \ret TCPHandle - handle to interface

 */
TCPHandle TCP_init();


/**
   \brief Listens for new connections

   \param handl - Handle to interface initialized in initTCP()-function
   \ret - Handle to interface used in communicating with client (different than the one gotten from initTCP()
*/
TCPHandle TCP_listen(TCPHandle handl);

/**
   \brief Answers to client's queries
   

   \param cliHandl - handle to client interface gotten from listenTCP() function

   \return 0
*/
int TCP_answerToClient(TCPHandle cliHandl);

/**
   \brief Add data for graph

   Data is stored in ring buffer. If buffer is full the oldest value is removed and replaced with the newest value

   \param data - data-to-be added to interface
*/
void TCP_pushGraphData(GraphData data);

/**
   \brief Close TCP server
   TODO: combine with closeClientConnection() because the same

   \param servHandl - closes the interface pointed by this handle
*/
void TCP_closeTCPServer(TCPHandle servHandl);


/**
   \brief same as closeTCPServer()
*/
void TCP_closeClientConnection(TCPHandle cliHandl);

#endif
