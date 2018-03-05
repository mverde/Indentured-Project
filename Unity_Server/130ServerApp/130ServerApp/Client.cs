using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;

namespace _130ServerApp
{
    class Client
    {
        public int Index;
        public string IP;
        public TcpClient Socket;
        public NetworkStream myStream;
        private byte[] readBuff;


        public void Start()
        {
            Socket.SendBufferSize = 4096;
            Socket.ReceiveBufferSize = 4096;
            myStream = Socket.GetStream();
            Array.Resize(ref readBuff, Socket.ReceiveBufferSize);
            myStream.BeginRead(readBuff, 0, Socket.ReceiveBufferSize, OnReceiveData, null);
        }

        void CloseConnection()
        {
            Socket.Close();
            Socket = null;
            Console.WriteLine("User Disconnected: " + IP);
        }

        void OnReceiveData(IAsyncResult result)
        {
            try
            {
                int readBytes = myStream.EndRead(result);
                if (Socket == null)
                {
                    return;
                }
                if (readBytes <= 0)
                {
                    CloseConnection();
                    return;
                }

                byte[] newBytes = null;
                Array.Resize(ref newBytes, readBytes);
                Buffer.BlockCopy(readBuff, 0, newBytes, 0, readBytes);

                //handledata

                if (Socket == null)
                {
                    return;
                }

                myStream.BeginRead(readBuff, 0, Socket.ReceiveBufferSize, OnReceiveData, null);
            }
            catch(Exception ex)
            {
                CloseConnection();
                return;
            }

        }//end onReceiveData






    }//end Client Class
}
