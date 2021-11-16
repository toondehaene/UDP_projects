using System;
using System.Net;
using System.Net.Sockets;
using System.Reflection;
using System.Text;
using System.Threading;

//TODO: make send frequency settable. 
//TODO: more customization
namespace UDPsend_recv
{
    class Program
    {
        private const int Portnr = 20777;

        static void Main(string[] args)
        {
            ArduinoComm.DataIn = Encoding.ASCII.GetBytes("data_in_test");
            ArduinoComm.DataOut = Encoding.ASCII.GetBytes("data_out_test");

            var sendThread = new Thread(ArduinoComm.StartSend);
            var recvThread = new Thread(ArduinoComm.StartReceive);
            sendThread.Start();
            recvThread.Start();
            for (var i = 0; i < 5; i++)
            {
                Console.WriteLine("output string: ");
                ArduinoComm.DataOut = Encoding.ASCII.GetBytes(Console.ReadLine() ?? "null_input");
            }
        }
    }
}

public class ArduinoComm
{
    public static byte[] DataIn, DataOut;

    private static void StartReceive(int portNrRecv)
    {
        using var udpClientB = new UdpClient(portNrRecv);
        udpClientB.EnableBroadcast = true;
        while (true)
        {
            var ipep = new IPEndPoint(IPAddress.Any, portNrRecv);
            var buf = udpClientB.Receive(ref ipep);
            lock (DataIn)
            {
                DataIn = buf;
            }

            
            Console.WriteLine("data received, raw print:" + Encoding.ASCII.GetString(DataIn));
            // Thread.Sleep(500); //TODO: comment this out
        }
    }

    public static void StartReceive()
    {
        StartReceive(20666);
    }

    private static void StartSend(int portNrSend)
    {
        using var udpClientA = new UdpClient(portNrSend);
        udpClientA.EnableBroadcast = true;
        byte[] buf;
        while (true)
        {
            lock (DataOut)
            {
                buf = DataOut;
            }

            udpClientA.Send(buf, buf.Length, "192.168.137.255", portNrSend);
            // Console.WriteLine("one message sent");
            Thread.Sleep(200);
        }
    }

    public static void StartSend()
    {
        StartSend(20777);
    }
}