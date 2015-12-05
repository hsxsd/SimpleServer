using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace TradeServer
{
    namespace Tcp
    {
        /// <summary>
        /// 维护客户账号密码、客户端连接Socket、异步事件、通信缓冲区等信息，并负责与客户端通信
        /// </summary>
        class Client
        {
            /// <summary>
            /// 接收的客户端连接socket
            /// </summary>
            public Socket m_socket;

            /// <summary>Receive/Send异步事件池</summary>
            Stack<SocketAsyncEventArgs> m_readWriteEventArgPool;

            /// <summary>接收缓冲池</summary>
            BufferManager.BufferPool m_receiveBufferPool;

            /// <summary>发送缓冲缓冲池</summary>
            BufferManager.BufferPool m_sendBufferPool;

            /// <summary>互斥信号量，保证最多接收设定数目的客户端连接</summary>
            Semaphore m_maxNumberAcceptedClients;

            /// <summary>
            /// 异步操作事件，用于异步通信（Receive, Send操作）
            /// </summary>
            public SocketAsyncEventArgs m_readWriteEventArg;

            /// <summary>
            /// 与客户端通信的接收缓冲区
            /// </summary>
            public BufferManager.Buffer m_receiveBuffer;

            /// <summary>
            /// 与客户端通信的发送缓冲区
            /// </summary>
            public BufferManager.Buffer m_sendBuffer;

            public Client(Socket socket, Stack<SocketAsyncEventArgs> readWriteEventArgPool, BufferManager.BufferPool receiveBufferPool, BufferManager.BufferPool sendBufferPool, Semaphore maxNumberAcceptedClients)
            {
                m_socket                   = socket;
                m_readWriteEventArgPool    = readWriteEventArgPool;
                m_receiveBufferPool        = receiveBufferPool;
                m_sendBufferPool           = sendBufferPool;
                m_maxNumberAcceptedClients = maxNumberAcceptedClients;
                m_readWriteEventArg        = readWriteEventArgPool.Pop();
                m_receiveBuffer            = receiveBufferPool.GetBuffer();
                m_sendBuffer               = sendBufferPool.GetBuffer();

                // 绑定异步事件回调函数
                m_readWriteEventArg.Completed += IO_Completed;
            }

            public void Start()
            {
                // 设置接收事件的缓冲区
                m_readWriteEventArg.SetBuffer(m_receiveBuffer.bytes,
                                              m_receiveBuffer.offset,
                                              m_receiveBuffer.size);

                // 异步接收客户端数据
                bool willRaiseEvent = m_socket.ReceiveAsync(m_readWriteEventArg);
                if (!willRaiseEvent)
                {
                    ProcessReceive();
                }
            }

            /// <summary>
            /// 异步等待客户端IO完成
            /// </summary>
            private void IO_Completed(object sender, SocketAsyncEventArgs readWriteEventArg)
            {
                switch (readWriteEventArg.LastOperation)
                {
                    case SocketAsyncOperation.Receive:
                        ProcessReceive();
                        break;
                    case SocketAsyncOperation.Send:
                        ProcessSend();
                        break;
                    default:
                        throw new ArgumentException("The last operation completed on the socket was not a receive or send");
                }
            }

            /// <summary>
            /// 处理接收到的客户端数据
            /// </summary>
            private void ProcessReceive()
            {
                if (m_readWriteEventArg.BytesTransferred > 0 &&
                    m_readWriteEventArg.SocketError == SocketError.Success)
                {
                    // 处理接收到的数据
                    m_receiveBuffer.count = m_readWriteEventArg.BytesTransferred;
                    Console.WriteLine("接收到客户端数据：" + m_receiveBuffer.ToString());

                    // 填充发送缓冲区
                    string sendStr = "客户端你好";
                    System.Buffer.BlockCopy(Encoding.Default.GetBytes(sendStr), 0, m_sendBuffer.bytes, m_sendBuffer.offset, Encoding.Default.GetByteCount(sendStr));
                    m_sendBuffer.count = Encoding.Default.GetByteCount(sendStr);

                    // 设置发送缓冲区
                    m_readWriteEventArg.SetBuffer(m_sendBuffer.bytes, m_sendBuffer.offset, m_sendBuffer.count);

                    // 异步发送
                    bool willRaiseEvent = m_socket.SendAsync(m_readWriteEventArg);
                    if (!willRaiseEvent)
                    {
                        ProcessSend();
                    }
                }
                else
                {
                    Close();
                }
            }

            /// <summary>
            /// 完成向客户端发送数据任务
            /// </summary>
            private void ProcessSend()
            {
                if (m_readWriteEventArg.SocketError == SocketError.Success)
                {
                    // 开始下一次接收客户端数据
                    Start();
                }
                else
                {
                    Close();
                }
            }

            /// <summary>
            /// 关闭客户端连接，释放互斥信号，回收接收和发送缓冲区，回收异步读写事件
            /// </summary>
            private void Close()
            {
                try
                {
                    m_socket.Shutdown(SocketShutdown.Send);
                }
                catch (Exception) { }
                m_socket.Close();

                m_receiveBufferPool.FreeBuffer(m_receiveBuffer);
                m_sendBufferPool.FreeBuffer(m_sendBuffer);

                m_readWriteEventArgPool.Push(m_readWriteEventArg);

                m_maxNumberAcceptedClients.Release();
            }
        }
    }
}
