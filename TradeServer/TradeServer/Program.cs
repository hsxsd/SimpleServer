using System;
using System.Net;

namespace TradeServer
{
    class Program
    {
        static void Main(string[] args)
        {
            Tcp.TcpServer server = new Tcp.TcpServer(1000, 1000, new IPEndPoint(IPAddress.Any, 5555));
            server.Start();
            Console.ReadKey();
        }
    }
}
