#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "SocketConnections.h"
#include <ws2tcpip.h>
#include <map>
using namespace std;
#pragma comment (lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015" 
class Server
{
public:
	Server(void);
	~Server(void);

	//Socket to listen for clients 
	SOCKET ListenSocket;

	//socket for clients to communicate through 
	SOCKET ClientSocket;

	//check error vals 
	int iResult;

	//ID table 
	std::map<unsigned int, SOCKET> sessions;

	
	bool acceptNewClient(unsigned int & id);

	int receiveData(unsigned int client_id, char * recvbuf);
	void sendMessageToEachClient(char * packets, int totalSize);
};

