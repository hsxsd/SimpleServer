using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using TradeServer.Tcp;

namespace ClientDemo
{
    class Program
    {
        static void Main(string[] args)
        {
            Socket client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            try
            {
                client.Connect(new IPEndPoint(IPAddress.Parse("127.0.0.1"), 5555));
                Console.WriteLine("连接服务器成功");
            }
            catch
            {
                Console.WriteLine("连接服务器失败");
                return;
            }

            string sendstr = "你好服务端";
            TcpBuffer sendPack = TcpProtocol.Pack(new TcpBuffer(Encoding.Default.GetBytes(sendstr)), 0);

            client.Send(sendPack.bytes);
        }
    }
}
