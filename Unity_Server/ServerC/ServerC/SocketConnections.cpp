#include "SocketConnections.h"
#include "NetworkData.h"

//Winsock functions for sending and receiving messages 
int SocketConnections::sendMessage(SOCKET curSocket, char * message, int messageSize)
{
	return send(curSocket, message, messageSize, 0);
}

int SocketConnections::receiveMessage(SOCKET curSocket, char * buffer, int bufSize)
{
	return recv(curSocket, buffer, bufSize, 0);
}