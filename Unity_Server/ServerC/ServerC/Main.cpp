#include "GameServer.h"
#include <process.h>
#include <iostream>


void serverLoop(void *);
void clientLoop(void);

GameServer * server;

int main()
{
	server = new GameServer();
	//create thread with arbitrary argument for the run function
	//threads used to allow for multiple clients actions to run 
	_beginthread(serverLoop, 0, (void*)12);

	
	clientLoop();

}

void serverLoop(void * arg)
{
	while (true)
	{
		//cout << "guh";
		server->update();
	}
}
void clientLoop()
{
	while (true)
	{
		//If you want to update any game client stuff from the server end 
		//will later run client->update();
	}
}