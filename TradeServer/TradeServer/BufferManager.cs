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

            public Buffer(byte[] bytes, int offset, int size)
            {
                m_bytes  = bytes;
                m_size   = size;
                m_offset = offset;
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
        /// <remarks> 
        /// 1.维护一个缓冲池（预分配的一大块连续内存）。
        /// 2.缓冲池被分为若干个等宽缓冲区。
        /// 3.缓冲区状态分为可用和被占用。
        /// 4.缓冲池向外部提供可用缓冲区，直至所有缓冲区都被占用。
        /// 5.被占用的缓冲区经外部释放后将重新变为可用。
        /// </remarks>
        class BufferPool
        {
            byte[]     m_bytes;         // 缓冲池内存块
            int        m_bufferNum;     // 缓冲区个数
            int        m_bufferSize;    // 每个缓冲区的大小
            int        m_currentIndex;  // 下一个可用的缓冲区索引（相对于内存块首字节的索引）
            Stack<int> m_freeIndex;     // 经释放重新变为可用的缓冲区索引

            /// <summary>
            /// 创建缓冲池
            /// </summary>
            /// <param name="bufferNum">缓冲区个数</param>
            /// <param name="bufferSize">每个缓冲区的大小</param>
            public BufferPool(int bufferSize, int bufferNum)
            {
                m_bufferNum    = bufferNum;
                m_bufferSize   = bufferSize;
                m_bytes        = new byte[bufferSize * bufferNum];
                m_currentIndex = 0;
                m_freeIndex    = new Stack<int>();
            }

            private int TotalBytes
            {
                get { return m_bufferSize * m_bufferNum; }
            }

            public Buffer GetBuffer()
            {
                if (m_freeIndex.Count > 0)
                    return new Buffer(m_bytes, m_freeIndex.Pop(), m_bufferSize);
                else if (m_currentIndex + m_bufferSize < TotalBytes)
                    return new Buffer(m_bytes, m_currentIndex, m_bufferSize);
                else
                    return null;
            }

            public void FreeBuffer(Buffer buffer)
            {
                if (buffer != null)
                    m_freeIndex.Push(buffer.offset);
            }
        }
    }
}
