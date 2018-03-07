using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Net;

namespace _130ServerApp
{
    class network
    {
        public TcpListener ServerSocket;
        public static network instance = new network();
        public static Client[] Clients = new Client[100];

        public void ServerStart()
        {
            for(int i = 0; i < 100; i++)
            {
                Clients[i] = new Client();
            }
            ServerSocket = new TcpListener(IPAddress.Any, 7500);
            ServerSocket.Start();
            ServerSocket.BeginAcceptTcpClient(OnClientConnect, null);
            Console.WriteLine("Server has started.");
        }

        void OnClientConnect(IAsyncResult result)
        {
            TcpClient client = ServerSocket.EndAcceptTcpClient(result);
            client.NoDelay = false;
            ServerSocket.BeginAcceptTcpClient(OnClientConnect, null);

            for (int i = 1; i < 100; i++)
            {
                if (Clients[i].Socket == null)
                {
                    Clients[i].Socket = client;
                    Clients[i].Index = i;
                    Clients[i].IP = client.Client.RemoteEndPoint.ToString();
                    Clients[i].Start();
                    Console.WriteLine("Incoming Connection from " + Clients[i].IP + "|| Index: " + i);
                    //SendWelcomeMessages
                    return;
                }
            }
        }
    }
}
