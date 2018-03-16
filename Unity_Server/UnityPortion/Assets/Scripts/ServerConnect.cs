using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Net.Sockets;
using System.IO;
using System;

public class ServerConnect : MonoBehaviour {

    private static ServerConnect instance;

    [Header("Network Settings")]

    public string ServerIP = "127.0.0.1";
    public int ServerPort = 7500;
    public bool isConnected;

    public TcpClient PlayerSocket;
    public NetworkStream myStream;
    public StreamReader myReader;
    public StreamWriter myWriter;

    private byte[] asyncBuff;
    public bool shouldHandleData;

    private void Awake()
    {
        instance = this;
    }

    
	void Start () {
        connectToServer();
    }

    void connectToServer()
    {
        if (PlayerSocket != null)
        {
            if (PlayerSocket.Connected || isConnected)
                return;
            PlayerSocket.Close();
            PlayerSocket = null;
        }

        PlayerSocket = new TcpClient();
        PlayerSocket.ReceiveBufferSize = 4096;
        PlayerSocket.SendBufferSize = 4096;
        PlayerSocket.NoDelay = false;
        Array.Resize(ref asyncBuff, 8192);
        PlayerSocket.BeginConnect(ServerIP, ServerPort, new AsyncCallback(ConnectCallback), PlayerSocket);
        isConnected = true;
        MenuManager.instance._menu = MenuManager.Menu.Home;
    }

    void ConnectCallback(IAsyncResult result)
    {
        if(PlayerSocket != null)
        {
            PlayerSocket.EndConnect(result);
            if(PlayerSocket.Connected == false)
            {
                isConnected = false;
                return;
            }
            else
            {
                PlayerSocket.NoDelay = true;
                myStream = PlayerSocket.GetStream();
                myStream.BeginRead(asyncBuff, 0, 8192, OnReceive, null);
            }
        }
    }

    private void Update()
    {
        if (shouldHandleData)
        {
            //ClientHandleData.instance.HandleData(myBytes);
            //shouldHandleData = false;
        }
    }

    void OnReceive(IAsyncResult result)
    {
        if(PlayerSocket != null)
        {
            if (PlayerSocket == null)
                return;

            int byteArray = myStream.EndRead(result);
            byte[] myBytes = null;
            Array.Resize(ref myBytes, byteArray);
            Buffer.BlockCopy(asyncBuff, 0, myBytes, 0, byteArray);

            if(byteArray ==0)
            {
                Debug.Log("Disconnected from server.");
                PlayerSocket.Close();
                return;
            }


            shouldHandleData = true;

            if(PlayerSocket == null)
                return;

            myStream.BeginRead(asyncBuff, 0, 8192, OnReceive, null);

        }
    }


}
