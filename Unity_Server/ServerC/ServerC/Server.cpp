#include "Server.h"
#include "NetworkData.h"
#include <string.h>
#include <iostream>
#include <vector>
#include <string>
//#include <ServerCall.h>

Server::Server(void)
{
	WSADATA wsaData;

	//sockets for the server
	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;



	//address to listen to 
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	//Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		exit(1);
	}

	//address info
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;    
	hints.ai_flags = AI_PASSIVE;

	//result for address and port info - check for errors 
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		exit(1);
	}

	//listening socket to connect to the server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		exit(1);
	}

	//socket should be non blocking
	u_long iMode = 1;
	iResult = ioctlsocket(ListenSocket, FIONBIO, &iMode);

	if (iResult == SOCKET_ERROR) {
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	//TCP listen socket bind
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	//free up memory 
	freeaddrinfo(result);

	//look for new clients 
	iResult = listen(ListenSocket, SOMAXCONN);

	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
}


//For when a new client is attempting to connect
bool Server::acceptNewClient(unsigned int & id)
{
	//if client waiting, accept the connection and save the socket
	ClientSocket = accept(ListenSocket, NULL, NULL);

	if (ClientSocket != INVALID_SOCKET)
	{
		//disable nagle on the client's socket
		char value = 1;
		setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

		//insert client into ID table 
		sessions.insert(pair<unsigned int, SOCKET>(id, ClientSocket));

		return true;
	}

	return false;
}


//Used to send a message to every client
//in future, will want to send the message only to the one who sent it
void Server::sendMessageToEachClient(char * packets, int totalSize)
{
	SOCKET currentSocket;
	std::map<unsigned int, SOCKET>::iterator iter;
	int iSendResult;
	

	for (iter = sessions.begin(); iter != sessions.end(); iter++)
	{
		currentSocket = iter->second;
		iSendResult = SocketConnections::sendMessage(currentSocket, packets, totalSize);

		if (iSendResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(currentSocket);
		}
	}
}



//Function for Receiving Messages from Client
//We want to take the client's location (lat,long) along with any filters specified by the game
int Server::receiveData(unsigned int client_id, char * recvbuf)
{
	//go through every client 
	if (sessions.find(client_id) != sessions.end())
	{
		if (client_id == 0) //client 0 is the host 
			return 0;
		SOCKET currentSocket = sessions[client_id];

		//call the Winsock method to place the message inside a buffer
		iResult = SocketConnections::receiveMessage(currentSocket, recvbuf, MAX_PACKET_SIZE);
		


		//if a new message is ready 
		if (strlen(recvbuf) != 0 && recvbuf[0]!='\0')
		{
			//testing case: to make sure we can convert string to a double
			//char * tempd = "30.19452,32.29412";
			//string str(tempd);

			//conver the coordinate to two doubles 
			string str(recvbuf);
			string lat = "";
			string lon = "";
			bool BeforeComma = true;
			for (int i = 0; i < str.length(); i++)
			{
				if (str[i] == ',')
				{
					BeforeComma = false;
					continue;
				}

				if (BeforeComma)
					lat += (str[i]);
				else
					lon += (str[i]);
					
			}
			
			//cout << "lon: " << lon << endl;
			//cout << "lat: " << lat << endl;

			std::string::size_type sz;

			double longitude = std::stod(lon,&sz);
			double latitude = std::stod(lat, &sz);

			//cout << "long double: " << longitude << endl;
			//cout << "lat double: " << latitude << endl;
			const unsigned int packet_size = sizeof(Packet);



			//CALL C++ API HERE
			//This is an example call from the Location Database Query 
			/*
			ServerCall test = ServerCall();
			vector<Place> poi;
			poi = test.SearchByCoordinate(latitude, longitude, 20000, 5, "");

			for (unsigned int i = 0; i < poi.size(); i++)
			{
				char * outsend = placeToString(poi[i]);
				sendMessageToEachClient(outsend, packet_size);
			}
			*/
			//cout << endl;
			cout << "got message from client: " << recvbuf << endl;
			cout << "sending query response to client";
			recvbuf[0] = '\0';


			//Delete this later. Used for Demo/Testing
			char * outsend = "Message Received. Here is a confirmation";
			sendMessageToEachClient(outsend, packet_size);
			
		}

		//attempt to receive but client is no longer available 
		if (iResult == 0)
		{
			printf("Connection closed\n");
			closesocket(currentSocket);
			recvbuf = "";
		}
		//printf("iresult: ", iResult);
		return iResult;
	}
	return 0;
}