using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class testingScript : MonoBehaviour
{

    private static Vector2 currentPos;
    public static string toServer = "";

    private string serverMsg;
    public string msgToServer;
    public Text retText;

    public bool UseCoordinates = true;


    public static string fromServer = "Message From Server: ";

    void Start()
    {
        //Starts at 0 by def
        //another function is called outside of this class to set these to your gps coordinates
        currentPos.x = 0f;
        currentPos.y = 0f;
    }

    void Update()
    {
        //convert them to string and make sure your output is how you want it 
        //csv style 
        toServer = currentPos.x.ToString("F5") + ',' + currentPos.y.ToString("F5");
        Debug.Log("Sending this message to server: " + toServer);
    }

    //test sending a message to the server using our tcp socket 
    public void SendToServer(string str)
    {
        if (UseCoordinates)
            ;// myTCP.writeSocket(GameManager.toServer);
        else
            ;// myTCP.writeSocket(str);
        Debug.Log("[CLIENT] -> " + str);
    }

    //test receiving a message from the server using the tcp socket
    void SocketResponse()
    {
        string serverSays = "";// myTCP.readSocket();

        if (serverSays != "")
        {
            Debug.Log("[SERVER]" + serverSays);
            fromServer += ('\n' + serverSays);
            retText.text = fromServer;
        }
    }
}
