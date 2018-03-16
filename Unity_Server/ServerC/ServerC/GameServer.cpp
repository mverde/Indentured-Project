#include "GameServer.h"
#include "NetworkData.h"
#include <iostream>   
#include <string> 

unsigned int GameServer::client_id;

GameServer::GameServer(void)
{
	//client id is used to identify each client connected to the server
	//cliend 0 is the host (server)
	client_id = 0;

	//server object that will run and listen/store/do everything 
	network = new Server();
}

void GameServer::update()
{

	//look for new clients. Update ID list if added another
	if (network->acceptNewClient(client_id))
	{
		printf("client %d has been connected to the server\n", client_id);

		client_id++;
	}

	//check if there is any info your clients wanted to send to the server
	receiveFromClients();
	
}

void GameServer::receiveFromClients()
{
	Packet packet;

	//iterate through all clients 
	std::map<unsigned int, SOCKET>::iterator iter;

	for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
	{
		//get the data from the client 
		int data_length = network->receiveData(iter->first, network_data);

	
	}
}