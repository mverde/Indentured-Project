#include "Server.h"

class GameServer
{

public:

	GameServer(void);
	~GameServer(void);

	void update();

private:

	// IDs for the clients connecting for table in ServerNetwork 
	static unsigned int client_id;

	// The ServerNetwork object 
	Server* network;
};