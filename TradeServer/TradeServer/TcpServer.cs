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
            /// <summary>最大并发连接数</summary>
            private int m_numConnections;

            /// <summary>接收缓冲区大小</summary>
            private int m_receiveBufferSize;

            /// <summary>发送缓冲区大小</summary>
            private int m_sendBufferSize;

            /// <summary>接收/发送缓冲池</summary>
            BufferManager.BufferPool m_readWriteBufferPool;

            /// <summary>监听Socket</summary>
            Socket m_listenSocket;

            /// <summary>Accept异步事件</summary>
            SocketAsyncEventArgs m_acceptEventArg;

            /// <summary>Receive/Send异步事件池</summary>
            EventArgsPool m_readWriteEventArgPool;

            /// <summary>互斥信号量，保证最多接收设定数目的客户端连接</summary>
            Semaphore m_maxNumberAcceptedClients;

            /// <summary>
            /// 创建Tcp服务器，绑定IP和端口号，并使其处于监听状态
            /// </summary>
            /// <param name="numConnections">允许客户端同时接入的最大数目</param>
            /// <param name="receiveBufferSize">接收缓冲区大小</param>
            /// <param name="sendBufferSize">发送缓冲区大小</param>
            /// <param name="serverEndPoint">服务端绑定的IP地址和端口号</param>
            public TcpServer(int numConnections, int receiveBufferSize, int sendBufferSize, IPEndPoint serverEndPoint)
            {
                m_numConnections           = numConnections;        // 初始化最大连接数
                m_receiveBufferSize        = receiveBufferSize;     // 初始化接收缓冲区大小
                m_sendBufferSize           = sendBufferSize;        // 初始化发送缓冲区大小
                m_readWriteBufferPool      = new BufferManager.BufferPool(m_receiveBufferSize, numConnections * 2);   // 创建接收缓冲池，每个连接用到2个缓冲区（一个接收缓冲区，一个发送缓冲区）
                m_maxNumberAcceptedClients = new Semaphore(numConnections, numConnections);                           // 创建互斥信号量

                // 初始化接收连接异步操作事件
                m_acceptEventArg = new SocketAsyncEventArgs(); 
                m_acceptEventArg.Completed += Accept_Completed;

                // 初始化收发数据异步操作事件池
                m_readWriteEventArgPool = new EventArgsPool(numConnections);

                // 创建监听socket
                m_listenSocket = new Socket(serverEndPoint.AddressFamily,
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
                // 等待可以接收新客户端连接的信号
                // 通过信号量确保连接数不超过设定数目
                m_maxNumberAcceptedClients.WaitOne();

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
                Client client = new Client(acceptEventArg.AcceptSocket, m_readWriteEventArgPool, m_readWriteBufferPool, m_maxNumberAcceptedClients);
                client.Start();

                // 清理本次客户端连接的socket，并继续接收下一个客户端连接
                m_acceptEventArg.AcceptSocket = null;
                Start();
            }
        }
    }
}
