#include "communication.h"

// 处理客户端消息的线程
DWORD WINAPI	tcper_InstanceThread(LPVOID); 

// 压缩解压缩函数
typedef unsigned char BYTE;
/* data 原数据 ndata 原数据长度 zdata 压缩后数据 nzdata 压缩后长度 */
int tcper_gzcompress(Bytef *zdata, uLong *nzdata, Bytef *data, uLong ndata) 
{
	z_stream c_stream;
	int err = 0;

	if (data && ndata > 0) {
		c_stream.zalloc = NULL;
		c_stream.zfree = NULL;
		c_stream.opaque = NULL;
		//只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
		if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
			MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK)
			return -1;
		c_stream.next_in = data;
		c_stream.avail_in = ndata;
		c_stream.next_out = zdata;
		c_stream.avail_out = *nzdata;
		while (c_stream.avail_in != 0 && c_stream.total_out < *nzdata) {
			if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK)
				return -1;
		}
		if (c_stream.avail_in != 0)
			return c_stream.avail_in;
		for (;;) {
			if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END)
				break;
			if (err != Z_OK)
				return -1;
		}
		if (deflateEnd(&c_stream) != Z_OK)
			return -1;
		*nzdata = c_stream.total_out;
		return 0;
	}
	return -1;
}

/* Uncompress gzip data */
/* zdata 数据 nzdata 原数据长度 data 解压后数据 ndata 解压后长度 */
int tcper_gzuncompress(Byte *data, uLong *ndata, Byte *zdata, uLong nzdata) {
	int err = 0;
	z_stream d_stream = { 0 }; /* decompression stream */
	static char dummy_head[2] = { 0x8 + 0x7 * 0x10, (((0x8 + 0x7 * 0x10) * 0x100
		+ 30) / 31 * 31) & 0xFF, };
	d_stream.zalloc = NULL;
	d_stream.zfree = NULL;
	d_stream.opaque = NULL;
	d_stream.next_in = zdata;
	d_stream.avail_in = 0;
	d_stream.next_out = data;
	//只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
	if (inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK)
		return -1;
	//if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
	while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END)
			break;
		if (err != Z_OK) {
			if (err == Z_DATA_ERROR) {
				d_stream.next_in = (Bytef*) dummy_head;
				d_stream.avail_in = sizeof(dummy_head);
				if ((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
					return -1;
				}
			} else
				return -1;
		}
	}
	if (inflateEnd(&d_stream) != Z_OK)
		return -1;
	*ndata = d_stream.total_out;
	return 0;
}

// 消息打包函数
BYTE *tcper_PackCmd(int Command,Bytef *buffer,int bufferlen,int &enLen)
{
	
	BYTE buff[10 * 1024] = { 0 }; 
	unsigned int long bufLen=10*1024;
	int r1=tcper_gzcompress(buff,&bufLen,buffer,bufferlen);
	if (r1!=Z_OK)
		return NULL;
	/*	
	BYTE *buff = buffer;
	unsigned int long bufLen=bufferlen;
	*/
	BYTE *EnBuffer=new BYTE[bufLen+10];
	EnBuffer[0]='[';
	int *Cmd = (int*) &EnBuffer[1];
	*Cmd = Command;
	int *Count = (int*) &EnBuffer[5];
	*Count = bufLen;
	memcpy(&EnBuffer[9],buff,bufLen);
	EnBuffer[9+bufLen]=']';
	enLen=bufLen+10;
	return EnBuffer;
}

int tcper_start()
{	
	// 初始化服务端Socket
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	printf("准备启动Socket服务...\n");
	wVersionRequested = MAKEWORD( 1, 1 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		printf("启动Socket服务失败\n");
		return 0;		
	}

	if ( LOBYTE( wsaData.wVersion ) != 1 ||	HIBYTE( wsaData.wVersion ) != 1 ) {
		printf("Socket版本错误\n");
		WSACleanup( );
		return 0; 
	}
	printf("Socket服务启动完毕！\n\n");

	// 创建服务端Socket
	printf("准备创建监听socket...\n");
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);	
	if (sockSrv == INVALID_SOCKET)
	{
		printf("Socket创建失败\n");
		WSACleanup( );
		return 0; 
	}
	printf("监听socket创建完毕！\n\n");

	// 配置服务端Socket IP和端口
	int tcpPort = 5555;
	char *tcpPorts=get_config("tcpPort",20,"config");
	if (strlen(tcpPorts) != 0)
	{
		tcpPort = atoi(tcpPorts);
	}
	
	delete [] tcpPorts;
	printf("准备绑定IP和端口...\n");
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY); 
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(tcpPort);

	bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)); 
	printf("绑定完毕！IP为本机IP，端口号为%d\n\n", tcpPort);


	// 服务端Socket开启监听
	printf("开始监听客户端连接请求...\n\n");
	listen(sockSrv, 20);

	// 循环接收客户端连接
	// 每接收一个客户端，创建一个新线程处理消息
	int i = 1;
	while(true)
	{
		// 等待客户端socket连接
		printf("循环%d：等待客户端连接...\n\n", i);
		SOCKADDR_IN  addrClient;
		int len = sizeof(SOCKADDR);
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
		printf("循环%d：客户端接入，socket=%d\n\n", i, sockConn);

		// 创建处理线程
		printf("循环%d：为客户端%d创建处理线程...\n\n", i, sockConn);
		DWORD dwThreadId = 0; 
		ThreadData sockthread;
		sockthread.sockConn=sockConn;
		HANDLE hThread = CreateThread(	NULL, 1024*1024*20, tcper_InstanceThread, (LPVOID)&sockthread, 0, &dwThreadId);

		if (hThread == NULL) {
			printf("线程创建失败，错误码：%d\n", GetLastError()); 
			return 0;
		}

		CloseHandle(hThread);	// 关闭线程句柄		

		i++;
	}
}

DWORD WINAPI tcper_InstanceThread(LPVOID lpvParam) // 处理线程函数，函数参数为客户端的Socket
{
	DWORD  dwThreadId = GetCurrentThreadId(); 
	//UserList current_user = NULL;
	
	printf("子线程%d: 线程创建完毕！\n\n", dwThreadId);

	// 参数为客户端Socket
	ThreadData *sockthread = (ThreadData*) lpvParam; 
	SOCKET sockConn=sockthread->sockConn;
	while(1)
	{

		// 接收客户端数据

		// 接收消息头
		printf("子线程%d: 开始接收消息头...\n", dwThreadId);

		char recv_header[50] = { 0 };
		int hlen = sizeof(char) + sizeof(int) + sizeof(int);	// [ command size
		int len = recv(sockConn, recv_header, hlen, 0);

		if (len == 0)
		{
			printf("子线程%d: 客户端断开连接\n", dwThreadId);
			printf("子线程%d: 关闭客户端Socket %d\n", dwThreadId, sockConn);
			closesocket(sockConn);

			printf("子线程%d: 线程退出\n", dwThreadId);
			return 0;   
		}

		if (len!=hlen)
		{
			printf("子线程%d: 接收消息头错误 len = %d\n", dwThreadId, len);
			printf("子线程%d: 关闭客户端Socket %d\n", dwThreadId, sockConn);
			closesocket(sockConn);
			return 0;   
		}

		if (recv_header[0] != '[') 
		{
			printf("子线程%d: 消息头首字符不是[\n", dwThreadId);
			printf("子线程%d: 关闭客户端Socket %d\n", dwThreadId, sockConn);
			closesocket(sockConn);

			printf("子线程%d: 线程退出\n", dwThreadId);
			return 0;
		}

		// 获取指令类型
		int *pcommand = (int*)&recv_header[1];
		int command = *pcommand;

		// 获取数据包长度
		int *psize = (int*)&recv_header[5];
		int size = *psize;

		printf("子线程%d: 消息头接收完毕！消息类型：%d, 消息体大小：%d\n\n", dwThreadId, command_string[command], size);

		// 接收数据包
		// 接收长度 = 数据包长度 + 1（结尾']'字符）
		printf("子线程%d: 开始接收消息体...\n", dwThreadId);

		int elen = size + 1;
		int fcur=0;
		unsigned char recvbuffer[80*1024];
		memset(recvbuffer, 0, 80*1024);
		while(fcur<elen)
		{
			int len=recv(sockConn,(char *)&recvbuffer[fcur],elen-fcur,0);
			if (len <0)
			{ 
				printf("子线程%d: 接收数据包错误\n", dwThreadId);
				printf("子线程%d: 关闭客户端Socket %d\n", dwThreadId, sockConn);
				closesocket(sockConn);

				printf("子线程%d: 线程退出\n", dwThreadId);
				return 0;
			}
			else
				fcur = fcur +len;
		}

		if (fcur!=elen)
		{
			printf("子线程%d: 接收消息体错误，接收长度%d != 预期长度 %d\n", dwThreadId, fcur, elen);
			printf("子线程%d: 关闭客户端Socket %d\n", dwThreadId, sockConn);
			closesocket(sockConn);

			printf("子线程%d: 线程退出\n", dwThreadId);
			return 0;
		}
		printf("子线程%d: 接收消息体完毕！消息体长度为%d+1\n\n", dwThreadId, fcur-1);

		// 检查数据尾
		if (recvbuffer[elen-1]!=']')
		{
			printf("子线程%d: 消息尾不是]字符\n", dwThreadId);
			printf("子线程%d: 关闭客户端Socket %d\n", dwThreadId, sockConn);
			closesocket(sockConn);

			printf("子线程%d: 线程退出\n", dwThreadId);
			return 0;
		}

		// 解压数据包
		printf("子线程%d: 开始解压消息体...\n", dwThreadId);
		unsigned char debuffer[1024*1024];
		unsigned int long delen=1024*1024;
		int  err=tcper_gzuncompress(debuffer,&delen,recvbuffer,elen-1);

		if(err!=0)
		{
			printf("子线程%d: 解压数据包错误\n", dwThreadId);
			printf("子线程%d: 关闭客户端Socket %d\n", dwThreadId, sockConn);
			closesocket(sockConn);

			printf("子线程%d: 线程退出\n", dwThreadId);
			return 0;
		}

		printf("子线程%d: 解压完毕！大小%d -> %d\n\n", dwThreadId, elen-1, delen);


		unsigned char sendbuffer[80*1024];
		int sendlen = 0;
		int sendcommand = 0;

		// 打包返回结果

		handle_msg(command, (char*)debuffer, delen, (char*)sendbuffer, sendlen, sendcommand, dwThreadId);

		BYTE *pack = NULL;
		int packlen = 0;
		pack = tcper_PackCmd(sendcommand, (BYTE*)sendbuffer, sendlen, packlen);
		printf("子线程%d: 打包返回结果，长度%d\n", dwThreadId, packlen);

		// 发送打包结果
		if(send(sockConn, (char*)pack, packlen, 0) == SOCKET_ERROR)
		{
			printf("子线程%d: 发送打包结果错误\n", dwThreadId);
			delete []pack;
			closesocket(sockConn);
			return 0;
		}

		delete []pack;
	}

	// 断开连接，关闭Socket
	closesocket(sockConn);
	return 1;
}