#include "GameServer.h"
#include "NetworkData.h"
#include <iostream>   // std::cout
#include <string> 

unsigned int GameServer::client_id;

GameServer::GameServer(void)
{
	// id's to assign clients for our table
	client_id = 0;

	// set up the server network to listen 
	network = new Server();
}

void GameServer::update()
{

	// get new clients
	if (network->acceptNewClient(client_id))
	{
		printf("client %d has been connected to the server\n", client_id);

		client_id++;
	}
	receiveFromClients();
	//int retVal = recv
		//ConnectionreceiveMessage(currentSocket, recvbuf, MAX_PACKET_SIZE);
}

void GameServer::receiveFromClients()
{
	Packet packet;

	// go through all clients
	std::map<unsigned int, SOCKET>::iterator iter;

	for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
	{
		// get data for that client
		int data_length = network->receiveData(iter->first, network_data);

	
	}
}