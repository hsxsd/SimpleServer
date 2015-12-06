using System.Net.Sockets;
using System.Threading;
using System.Net;
using System.Collections.Generic;

namespace TradeServer
{
    namespace Tcp
    {
        /// <summary>
        /// Tcp服务器通信模块，接收客户端请求，并返回请求结果。
        /// </summary>
        class TcpServer
        {
            /// <summary>接收缓冲区大小</summary>
            private int          m_receiveBufferSize;

            /// <summary>发送缓冲区大小</summary>
            private int          m_sendBufferSize;

            /// <summary>监听Socket</summary>
            Socket               m_listenSocket;

            /// <summary>Accept异步事件</summary>
            SocketAsyncEventArgs m_acceptEventArg;


            /// <summary>
            /// 创建Tcp服务器，绑定IP和端口号，并使其处于监听状态
            /// </summary>
            /// <param name="receiveBufferSize">接收缓冲区大小</param>
            /// <param name="sendBufferSize">发送缓冲区大小</param>
            /// <param name="serverEndPoint">服务端绑定的IP地址和端口号</param>
            public TcpServer(int receiveBufferSize, int sendBufferSize, IPEndPoint serverEndPoint)
            {
                m_receiveBufferSize        = receiveBufferSize;     // 初始化接收缓冲区大小
                m_sendBufferSize           = sendBufferSize;        // 初始化发送缓冲区大小

                // 初始化接收连接异步操作事件
                m_acceptEventArg           = new SocketAsyncEventArgs(); 
                m_acceptEventArg.Completed += Accept_Completed;                

                // 创建监听socket
                m_listenSocket             = new Socket(serverEndPoint.AddressFamily,
                                                        SocketType.Stream,
                                                        ProtocolType.Tcp);

                // 绑定IP和端口
                m_listenSocket.Bind(serverEndPoint);

                // 监听
                m_listenSocket.Listen(100);
            }


            /// <summary>
            /// 开始接收客户端连接
            /// </summary>
            public void Start()
            {
                // 异步等待接收客户端连接
                bool willRaiseEvent = m_listenSocket.AcceptAsync(m_acceptEventArg);
                if (!willRaiseEvent)
                {
                    ProcessAccept(m_acceptEventArg);
                }
            }


            /// <summary>
            /// 异步等待接收客户端连接完成，一个新客户端已接入
            /// </summary>
            /// <param name="sender"></param>
            /// <param name="acceptEventArg"></param>
            private void Accept_Completed(object sender, SocketAsyncEventArgs acceptEventArg)
            {
                ProcessAccept(acceptEventArg);
            }


            /// <summary>
            /// 处理新接收的客户端
            /// </summary>
            /// <param name="acceptEventArg"></param>
            private void ProcessAccept(SocketAsyncEventArgs acceptEventArg)
            {
                // 创建并启动客户端
                Client client = new Client(acceptEventArg.AcceptSocket, m_receiveBufferSize, m_sendBufferSize);
                client.Start();

                // 清理本次客户端连接的socket，并继续接收下一个客户端连接
                m_acceptEventArg.AcceptSocket = null;
                Start();
            }
        }
    }
}
