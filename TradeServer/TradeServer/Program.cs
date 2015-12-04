using System;
using System.Net;

namespace TradeServer
{
    class Program
    {
        static void Main(string[] args)
        {
            Tcp.TcpServer server = new Tcp.TcpServer(10, 1000, 1000);
            server.Start(new IPEndPoint(IPAddress.Any, 5555));
            Console.ReadKey();
        }
    }
}
