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

	
	static unsigned int client_id;

	//network data - memory info etc 
	Server* network;
	
	//buffer
	char network_data[MAX_PACKET_SIZE];
};