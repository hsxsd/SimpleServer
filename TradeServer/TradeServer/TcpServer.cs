using System;
using System.Text;
using System.Net.Sockets;
using System.Threading;
using System.Net;

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

            /// <summary>每个连接的接收缓冲区大小</summary>
            private int m_receiveBufferSize;

            /// <summary>每个连接的发送缓冲区大小</summary>
            private int m_sendBufferSize;

            /// <summary>为所有连接提供接收缓冲区的缓冲池</summary>
            BufferManager.BufferPool m_receiveBufferPool;

            /// <summary>为所有连接提供发送缓冲区的缓冲池</summary>
            BufferManager.BufferPool m_sendBufferPool;

            /// <summary>监听Socket</summary>
            Socket m_listenSocket;

            /// <summary>异步操作事件，用于接收客户端连接（Accept操作）</summary>
            SocketAsyncEventArgs m_acceptEventArg;

            /// <summary>异步操作事件池，用于客户端异步通信（Receive, Send操作）</summary>
            AsyncEventArgsPool m_readWriteEventArgPool;

            /// <summary>用于线程互斥的信号量，保证最多接收设定数目的客户端连接</summary>
            Semaphore m_maxNumberAcceptedClients;

            /// <summary>
            /// 创建Tcp服务器
            /// </summary>
            /// <param name="numConnections">允许客户端同时接入的最大数目</param>
            /// <param name="receiveBufferSize">与客户端通信的接收缓冲区大小</param>
            /// <param name="sendBufferSize">与客户端通信的发送缓冲区大小</param>
            public TcpServer(int numConnections, int receiveBufferSize, int sendBufferSize)
            {
                m_numConnections           = numConnections;        // 最大连接数
                m_receiveBufferSize        = receiveBufferSize;     // 接收缓冲区大小
                m_sendBufferSize           = sendBufferSize;        // 发送缓冲区大小
                m_receiveBufferPool        = new BufferManager.BufferPool(m_receiveBufferSize, numConnections); // 接收缓冲区
                m_sendBufferPool           = new BufferManager.BufferPool(m_sendBufferSize, numConnections);    // 发送缓冲区
                m_maxNumberAcceptedClients = new Semaphore(numConnections, numConnections);                     // 互斥信号量，以保证不超过最大连接数

                // 初始化接收连接异步操作事件
                m_acceptEventArg = new SocketAsyncEventArgs();  // 创建异步事件
                m_acceptEventArg.Completed += Accept_Completed; // 绑定回调函数

                // 初始化收发数据异步操作事件池
                m_readWriteEventArgPool = new AsyncEventArgsPool(numConnections);
                for (int i = 0; i < m_numConnections; i++)
                {
                    // 创建异步事件
                    SocketAsyncEventArgs readWriteEventArg = new SocketAsyncEventArgs();

                    // 绑定回调函数
                    readWriteEventArg.Completed += new EventHandler<SocketAsyncEventArgs>(IO_Completed);

                    // 分配接收和发送缓冲区，存放到UserToken里
                    UserToken userToken         = new UserToken();
                    userToken.receiveBuffer     = m_receiveBufferPool.GetBuffer();
                    userToken.sendBuffer        = m_sendBufferPool.GetBuffer();
                    readWriteEventArg.UserToken = userToken;

                    // 把事件存放到事件池中
                    m_readWriteEventArgPool.Push(readWriteEventArg);
                }
            }

            /// <summary>
            /// 开启Tcp服务端：绑定IP，监听，接收客户端连接
            /// </summary>
            /// <param name="serverEndPoint">服务端绑定的IP地址和端口号</param>
            public void Start(IPEndPoint serverEndPoint)
            {
                // 创建监听socket
                m_listenSocket = new Socket(serverEndPoint.AddressFamily,
                                            SocketType.Stream,
                                            ProtocolType.Tcp);

                // 绑定IP和端口
                m_listenSocket.Bind(serverEndPoint);

                // 监听
                m_listenSocket.Listen(100);

                // 接收客户端连接
                StartAccept();
            }

            private void StartAccept()
            {
                // 清理上次客户端连接的socket
                if (m_acceptEventArg.AcceptSocket != null)
                    m_acceptEventArg.AcceptSocket = null;

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
                // 从事件池中取一个事件
                SocketAsyncEventArgs readWriteEventArg = m_readWriteEventArgPool.Pop();
                UserToken userToken = readWriteEventArg.UserToken as UserToken;

                // 保存socket
                userToken.socket = acceptEventArg.AcceptSocket;

                // 设置事件的接收缓冲区（因为处理的第一个事件是接收事件）
                readWriteEventArg.SetBuffer(userToken.receiveBuffer.bytes,
                                            userToken.receiveBuffer.offset,
                                            userToken.receiveBuffer.size);

                // 异步接收客户端的数据
                bool willRaiseEvent = userToken.socket.ReceiveAsync(readWriteEventArg);
                if (!willRaiseEvent)
                {
                    ProcessReceive(readWriteEventArg);
                }

                // 继续接收下一个客户端连接
                StartAccept();
            }

            /// <summary>
            /// 异步等待客户端IO完成
            /// </summary>
            /// <param name="sender"></param>
            /// <param name="readWriteEventArg"></param>
            private void IO_Completed(object sender, SocketAsyncEventArgs readWriteEventArg)
            {
                switch (readWriteEventArg.LastOperation)
                {
                    case SocketAsyncOperation.Receive:
                        ProcessReceive(readWriteEventArg);
                        break;
                    case SocketAsyncOperation.Send:
                        ProcessSend(readWriteEventArg);
                        break;
                    default:
                        throw new ArgumentException("The last operation completed on the socket was not a receive or send");
                }
            }

            /// <summary>
            /// 处理接收到的客户端数据
            /// </summary>
            /// <param name="readWriteEventArg"></param>
            private void ProcessReceive(SocketAsyncEventArgs readWriteEventArg)
            {
                if (readWriteEventArg.BytesTransferred > 0 &&
                    readWriteEventArg.SocketError == SocketError.Success)
                {
                    // 处理接收到的数据
                    UserToken userToken = readWriteEventArg.UserToken as UserToken;
                    BufferManager.Buffer receiveBuffer = userToken.receiveBuffer;
                    receiveBuffer.count = readWriteEventArg.BytesTransferred;
                    Console.WriteLine("接收到客户端数据：" + receiveBuffer.ToString());

                    // 填充发送缓冲区
                    string sendStr = "客户端你好";
                    BufferManager.Buffer sendBuffer = userToken.sendBuffer;
                    System.Buffer.BlockCopy(Encoding.Default.GetBytes(sendStr), 0, sendBuffer.bytes, sendBuffer.offset, Encoding.Default.GetByteCount(sendStr));
                    sendBuffer.count = Encoding.Default.GetByteCount(sendStr);

                    // 设置发送缓冲区
                    readWriteEventArg.SetBuffer(sendBuffer.bytes, sendBuffer.offset, sendBuffer.count);

                    // 异步发送
                    bool willRaiseEvent = userToken.socket.SendAsync(readWriteEventArg);
                    if (!willRaiseEvent)
                    {
                        ProcessSend(readWriteEventArg);
                    }
                }
                else
                {
                    CloseClient(readWriteEventArg);
                }
            }

            /// <summary>
            /// 完成向客户端发送数据任务
            /// </summary>
            /// <param name="readWriteEventArg"></param>
            private void ProcessSend(SocketAsyncEventArgs readWriteEventArg)
            {
                if (readWriteEventArg.SocketError == SocketError.Success)
                {
                    UserToken token = readWriteEventArg.UserToken as UserToken;

                    readWriteEventArg.SetBuffer(token.receiveBuffer.bytes, token.receiveBuffer.offset, token.receiveBuffer.size);
                    bool willRaiseEvent = token.socket.ReceiveAsync(readWriteEventArg);
                    if (!willRaiseEvent)
                    {
                        ProcessReceive(readWriteEventArg);
                    }
                }
                else
                {
                    CloseClient(readWriteEventArg);
                }
            }

            /// <summary>
            /// 关闭客户端连接，释放互斥信号，回收接收和发送缓冲区，回收异步读写事件
            /// </summary>
            /// <param name="readWriteEventArg"></param>
            private void CloseClient(SocketAsyncEventArgs readWriteEventArg)
            {
                UserToken token = readWriteEventArg.UserToken as UserToken;

                try
                {
                    token.socket.Shutdown(SocketShutdown.Send);
                }
                catch (Exception) { }
                token.socket.Close();

                m_receiveBufferPool.FreeBuffer(token.receiveBuffer);
                m_sendBufferPool.FreeBuffer(token.sendBuffer);

                m_maxNumberAcceptedClients.Release();

                m_readWriteEventArgPool.Push(readWriteEventArg);
            }
        }
    }
}
