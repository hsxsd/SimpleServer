using System.Net.Sockets;

namespace TradeServer
{
    namespace Tcp
    {
        /// <summary>
        /// 保存网络异步通信的相关信息
        /// </summary>
        class UserToken
        {
            /// <summary>
            /// 接收的客户端连接socket
            /// </summary>
            public Socket socket { get; set; }

            /// <summary>
            /// 与客户端通信的接收缓冲区
            /// </summary>
            public BufferManager.Buffer receiveBuffer { get; set; }

            /// <summary>
            /// 与客户端通信的发送缓冲区
            /// </summary>
            public BufferManager.Buffer sendBuffer { get; set; }
        }
    }
}
