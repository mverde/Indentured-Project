#include "GameServer.h"
#include <process.h>


void serverLoop(void *);
void clientLoop(void);

GameServer * server;
//ClientGame * client;
int main()
{
	server = new GameServer();
	// create thread with arbitrary argument for the run function
	_beginthread(serverLoop, 0, (void*)12);

	// initialize the client
	//client = new ClientGame();
	clientLoop();

}

void serverLoop(void * arg)
{
	while (true)
	{
		server->update();
	}
}
void clientLoop()
{
	while (true)
	{
		//do game stuff
		//will later run client->update();
	}
}