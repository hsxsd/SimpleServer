using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ICSharpCode.SharpZipLib.GZip;

namespace TradeServer
{
    namespace Tcp
    {
        public class TcpProtocol
        {
            private static byte[] DecompressByte(byte[] inBytes) //解压
            {
                byte[] buffer = new byte[0x800];
                MemoryStream baseInputStream = new MemoryStream(inBytes);
                Stream stream2 = new GZipInputStream(baseInputStream);
                MemoryStream stream3 = new MemoryStream();
                try
                {
                    while (true)
                    {
                        int count = stream2.Read(buffer, 0, buffer.Length);
                        if (count <= 0)
                        {
                            break;
                        }
                        stream3.Write(buffer, 0, count);
                    }
                    byte[] buffer2 = stream3.ToArray();
                    return buffer2;
                }
                finally
                {
                    stream3.Close();
                    stream2.Close();
                    baseInputStream.Close();
                }
            }

            private static byte[] CompressByte(byte[] inBytes)
            {
                MemoryStream baseOutputStream = new MemoryStream();
                Stream stream2 = new GZipOutputStream(baseOutputStream);
                stream2.Write(inBytes, 0, inBytes.Length);
                stream2.Close();
                byte[] buffer = baseOutputStream.ToArray();
                baseOutputStream.Close();
                return buffer;
            }

            public static TcpBuffer Unpack(TcpBuffer pack)
            {
                TcpBuffer unpack = null;

                if (pack != null && pack.count > 10)
                {
                    MemoryStream inputStream = new MemoryStream(pack.bytes, pack.offset, pack.count);
                    BinaryReader inputReader = new BinaryReader(inputStream);

                    char ch = inputReader.ReadChar(); //[，1字节
                    Int32 cmd = inputReader.ReadInt32(); //读取命令，4字节
                    int dataCount = inputReader.ReadInt32(); //读取数据包长度，4字节
                    if (inputReader.BaseStream.Length - inputReader.BaseStream.Position >= dataCount + 1)
                    {
                        byte[] dataBytes = inputReader.ReadBytes(dataCount);
                        char ch2 = inputReader.ReadChar();//]，1字节
                        if ((ch == '[') && (ch2 == ']'))
                        {
                            byte[] decompressBytes = DecompressByte(dataBytes);
                            unpack = new TcpBuffer(decompressBytes);
                        }
                    }

                    inputReader.Close();
                    inputStream.Close();
                }

                return unpack;
            }

            public static TcpBuffer Pack(TcpBuffer unpack, Int32 cmd)
            {
                TcpBuffer pack = null;

                if (unpack != null && unpack.count > 0)
                {
                    MemoryStream output = new MemoryStream();
                    BinaryWriter writer = new BinaryWriter(output);

                    byte [] buffer=CompressByte(unpack.bytes);
                    writer.Write('[');
                    writer.Write((int)cmd);
                    writer.Write(buffer.Length);
                    writer.Write(buffer);
                    writer.Write(']');
                    pack = new TcpBuffer(output.ToArray());

                    writer.Close();
                    output.Close();
                }

                return pack;
            }
        }
    }
}
