using System.Collections.Generic;
using System.Text;

namespace TradeServer
{
    // 缓冲区管理模块
    namespace BufferManager
    {
        /// <summary>
        /// 缓冲区
        /// </summary>
        class Buffer
        {
            byte[] m_bytes;
            int    m_offset;
            int    m_count;
            int    m_size;

            public Buffer(int size)
            {
                m_size   = size;
                m_bytes  = new byte[size];
                m_offset = 0;
                m_count  = 0;
            }

            /// <summary>
            /// 缓冲区所在的内存块
            /// </summary>
            public byte[] bytes
            {
                get { return m_bytes; }
            }

            /// <summary>
            /// 缓冲区相对于内存块首字节的索引
            /// </summary>
            public int offset
            {
                get { return m_offset; }
            }

            /// <summary>
            /// 缓冲区的大小
            /// </summary>
            public int size
            {
                get { return m_size; }
            }

            /// <summary>
            /// 缓冲区里实际内容的字节数
            /// </summary>
            public int count
            {
                get { return m_count; }
                set { m_count = value > m_size ? m_size : value; }
            }

            public override string ToString()
            {
                return Encoding.Default.GetString(m_bytes, m_offset, m_count);
            }
        }

        /// <summary>
        /// 提供或回收缓冲区。
        /// </summary>
        class BufferPool
        {
            int           m_bufferNum;     // 缓冲区个数
            int           m_bufferSize;    // 每个缓冲区的大小
            Stack<Buffer> m_bufferPool;    // 缓冲池

            /// <summary>
            /// 创建缓冲池
            /// </summary>
            /// <param name="bufferNum">缓冲区个数</param>
            /// <param name="bufferSize">每个缓冲区的大小</param>
            public BufferPool(int bufferSize, int bufferNum)
            {
                m_bufferNum    = bufferNum;
                m_bufferSize   = bufferSize;
                m_bufferPool   = new Stack<Buffer>(bufferNum);

                for (int i = 0; i < bufferNum; i++)
                {
                    m_bufferPool.Push(new Buffer(bufferSize));
                }
            }

            public Buffer GetBuffer()
            {
                return m_bufferPool.Pop();
            }

            public void FreeBuffer(Buffer buffer)
            {
                m_bufferPool.Push(buffer);
            }
        }
    }
}
