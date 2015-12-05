using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace TradeServer
{
    namespace Tcp
    {
        /// <summary>
        /// 提供或回收异步事件。
        /// </summary>
        class EventArgsPool
        {
            int                         m_eventArgsNum;     // 异步事件个数
            Stack<SocketAsyncEventArgs> m_eventArgsPool;    // 异步事件池

            /// <summary>
            /// 创建异步事件池
            /// </summary>
            /// <param name="eventArgsNum">异步事件个数</param>
            public EventArgsPool(int eventArgsNum)
            {
                m_eventArgsNum    = eventArgsNum;
                m_eventArgsPool   = new Stack<SocketAsyncEventArgs>(eventArgsNum);

                for (int i = 0; i < eventArgsNum; i++)
                {
                    m_eventArgsPool.Push(new SocketAsyncEventArgs());
                }
            }

            public SocketAsyncEventArgs GetEventArgs()
            {
                return m_eventArgsPool.Pop();
            }

            public void FreeEventArgs(SocketAsyncEventArgs eventArgs)
            {
                m_eventArgsPool.Push(eventArgs);
            }
        }
    }
}
