#include "Server.h"

#include <string.h>

#define MAX_PACKET_SIZE 1000000
class GameServer
{

public:

	GameServer(void);
	~GameServer(void);
	void receiveFromClients();

	void update();

private:

	// IDs for the clients connecting for table in ServerNetwork 
	static unsigned int client_id;

	// The ServerNetwork object 
	Server* network;
	// data buffer
	char network_data[MAX_PACKET_SIZE];
};