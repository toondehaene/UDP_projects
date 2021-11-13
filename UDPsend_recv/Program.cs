using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace UDPsend_recv
{
    class Program
    {
        private const int Portnr = 20777;

        static void Main(string[] args)
        {
            // This constructor arbitrarily assigns the local port number.
            UdpClient udpClient = new UdpClient(Portnr);
            try
            {
                var sendbcip = IPAddress.Parse("192.168.137.24");
                udpClient.Connect(sendbcip, Portnr);

                // Sends a message to the host to which you have connected.
                Byte[] sendBytes = Encoding.ASCII.GetBytes("Is anybody there?");

                udpClient.Send(sendBytes, sendBytes.Length);

                // Blocks until a message returns on this socket from a remote host.
                    Console.WriteLine("waiting on Receive()");

                    var listenRemoteip = new IPEndPoint(IPAddress.Parse("192.168.137.24"), Portnr);
                    Byte[] receiveBytes = udpClient.Receive(ref listenRemoteip);
                    Console.WriteLine("Received");
                    string returnData = Encoding.ASCII.GetString(receiveBytes);
                    
                 // Uses the IPEndPoint object to determine which of these two hosts responded.
                 Console.WriteLine("This is the message you received " +
                                   returnData.ToString());
                 Console.WriteLine("This message was sent from " +
                                   listenRemoteip.Address.ToString() +
                                   " on their port number " +
                listenRemoteip.Port.ToString());
            }
            catch (Exception e ) {
                Console.WriteLine(e.ToString());
            }
        }
    }
}

public class ArduinoComm
{
    private IPAddress _ipAddressOut;
    static byte[] dataIn, dataOut;
    private int portNrRecv, portNrSend;


    /**
     * portNr of PC and Arduino are expected to be the same
     */
    public ArduinoComm(IPAddress ipAddressOut = null, int portNrRecv = 20777, int portNrSend = 12345)
    {
        this.portNrRecv = portNrRecv;
        this.portNrSend = portNrSend;
        this._ipAddressOut = ipAddressOut ?? IPAddress.Parse("192.168.1.255");
    }

    public async Task StartReceive()
    {
        using var udpClientB = new UdpClient(portNrSend);
        while (true)
        {
            var recv = await udpClientB.ReceiveAsync();
            dataIn = recv.Buffer;
        }
    }
}