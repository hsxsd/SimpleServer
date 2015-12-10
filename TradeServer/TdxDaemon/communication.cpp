#include "communication.h"

// ����ͻ�����Ϣ���߳�
DWORD WINAPI	tcper_InstanceThread(LPVOID); 

// ѹ����ѹ������
typedef unsigned char BYTE;
/* data ԭ���� ndata ԭ���ݳ��� zdata ѹ�������� nzdata ѹ���󳤶� */
int tcper_gzcompress(Bytef *zdata, uLong *nzdata, Bytef *data, uLong ndata) 
{
	z_stream c_stream;
	int err = 0;

	if (data && ndata > 0) {
		c_stream.zalloc = NULL;
		c_stream.zfree = NULL;
		c_stream.opaque = NULL;
		//ֻ������ΪMAX_WBITS + 16��������ѹ���ı��д�header��trailer
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
/* zdata ���� nzdata ԭ���ݳ��� data ��ѹ������ ndata ��ѹ�󳤶� */
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
	//ֻ������ΪMAX_WBITS + 16�����ڽ�ѹ��header��trailer���ı�
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

// ��Ϣ�������
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
	// ��ʼ�������Socket
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	printf("׼������Socket����...\n");
	wVersionRequested = MAKEWORD( 1, 1 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		printf("����Socket����ʧ��\n");
		return 0;		
	}

	if ( LOBYTE( wsaData.wVersion ) != 1 ||	HIBYTE( wsaData.wVersion ) != 1 ) {
		printf("Socket�汾����\n");
		WSACleanup( );
		return 0; 
	}
	printf("Socket����������ϣ�\n\n");

	// ���������Socket
	printf("׼����������socket...\n");
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);	
	if (sockSrv == INVALID_SOCKET)
	{
		printf("Socket����ʧ��\n");
		WSACleanup( );
		return 0; 
	}
	printf("����socket������ϣ�\n\n");

	// ���÷����Socket IP�Ͷ˿�
	int tcpPort = 5555;
	char *tcpPorts=get_config("tcpPort",20,"config");
	if (strlen(tcpPorts) != 0)
	{
		tcpPort = atoi(tcpPorts);
	}
	
	delete [] tcpPorts;
	printf("׼����IP�Ͷ˿�...\n");
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY); 
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(tcpPort);

	bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)); 
	printf("����ϣ�IPΪ����IP���˿ں�Ϊ%d\n\n", tcpPort);


	// �����Socket��������
	printf("��ʼ�����ͻ�����������...\n\n");
	listen(sockSrv, 20);

	// ѭ�����տͻ�������
	// ÿ����һ���ͻ��ˣ�����һ�����̴߳�����Ϣ
	int i = 1;
	while(true)
	{
		// �ȴ��ͻ���socket����
		printf("ѭ��%d���ȴ��ͻ�������...\n\n", i);
		SOCKADDR_IN  addrClient;
		int len = sizeof(SOCKADDR);
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
		printf("ѭ��%d���ͻ��˽��룬socket=%d\n\n", i, sockConn);

		// ���������߳�
		printf("ѭ��%d��Ϊ�ͻ���%d���������߳�...\n\n", i, sockConn);
		DWORD dwThreadId = 0; 
		ThreadData sockthread;
		sockthread.sockConn=sockConn;
		HANDLE hThread = CreateThread(	NULL, 1024*1024*20, tcper_InstanceThread, (LPVOID)&sockthread, 0, &dwThreadId);

		if (hThread == NULL) {
			printf("�̴߳���ʧ�ܣ������룺%d\n", GetLastError()); 
			return 0;
		}

		CloseHandle(hThread);	// �ر��߳̾��		

		i++;
	}
}

DWORD WINAPI tcper_InstanceThread(LPVOID lpvParam) // �����̺߳�������������Ϊ�ͻ��˵�Socket
{
	DWORD  dwThreadId = GetCurrentThreadId(); 
	//UserList current_user = NULL;
	
	printf("���߳�%d: �̴߳�����ϣ�\n\n", dwThreadId);

	// ����Ϊ�ͻ���Socket
	ThreadData *sockthread = (ThreadData*) lpvParam; 
	SOCKET sockConn=sockthread->sockConn;
	while(1)
	{

		// ���տͻ�������

		// ������Ϣͷ
		printf("���߳�%d: ��ʼ������Ϣͷ...\n", dwThreadId);

		char recv_header[50] = { 0 };
		int hlen = sizeof(char) + sizeof(int) + sizeof(int);	// [ command size
		int len = recv(sockConn, recv_header, hlen, 0);

		if (len == 0)
		{
			printf("���߳�%d: �ͻ��˶Ͽ�����\n", dwThreadId);
			printf("���߳�%d: �رտͻ���Socket %d\n", dwThreadId, sockConn);
			closesocket(sockConn);

			printf("���߳�%d: �߳��˳�\n", dwThreadId);
			return 0;   
		}

		if (len!=hlen)
		{
			printf("���߳�%d: ������Ϣͷ���� len = %d\n", dwThreadId, len);
			printf("���߳�%d: �رտͻ���Socket %d\n", dwThreadId, sockConn);
			closesocket(sockConn);
			return 0;   
		}

		if (recv_header[0] != '[') 
		{
			printf("���߳�%d: ��Ϣͷ���ַ�����[\n", dwThreadId);
			printf("���߳�%d: �رտͻ���Socket %d\n", dwThreadId, sockConn);
			closesocket(sockConn);

			printf("���߳�%d: �߳��˳�\n", dwThreadId);
			return 0;
		}

		// ��ȡָ������
		int *pcommand = (int*)&recv_header[1];
		int command = *pcommand;

		// ��ȡ���ݰ�����
		int *psize = (int*)&recv_header[5];
		int size = *psize;

		printf("���߳�%d: ��Ϣͷ������ϣ���Ϣ���ͣ�%d, ��Ϣ���С��%d\n\n", dwThreadId, command_string[command], size);

		// �������ݰ�
		// ���ճ��� = ���ݰ����� + 1����β']'�ַ���
		printf("���߳�%d: ��ʼ������Ϣ��...\n", dwThreadId);

		int elen = size + 1;
		int fcur=0;
		unsigned char recvbuffer[80*1024];
		memset(recvbuffer, 0, 80*1024);
		while(fcur<elen)
		{
			int len=recv(sockConn,(char *)&recvbuffer[fcur],elen-fcur,0);
			if (len <0)
			{ 
				printf("���߳�%d: �������ݰ�����\n", dwThreadId);
				printf("���߳�%d: �رտͻ���Socket %d\n", dwThreadId, sockConn);
				closesocket(sockConn);

				printf("���߳�%d: �߳��˳�\n", dwThreadId);
				return 0;
			}
			else
				fcur = fcur +len;
		}

		if (fcur!=elen)
		{
			printf("���߳�%d: ������Ϣ����󣬽��ճ���%d != Ԥ�ڳ��� %d\n", dwThreadId, fcur, elen);
			printf("���߳�%d: �رտͻ���Socket %d\n", dwThreadId, sockConn);
			closesocket(sockConn);

			printf("���߳�%d: �߳��˳�\n", dwThreadId);
			return 0;
		}
		printf("���߳�%d: ������Ϣ����ϣ���Ϣ�峤��Ϊ%d+1\n\n", dwThreadId, fcur-1);

		// �������β
		if (recvbuffer[elen-1]!=']')
		{
			printf("���߳�%d: ��Ϣβ����]�ַ�\n", dwThreadId);
			printf("���߳�%d: �رտͻ���Socket %d\n", dwThreadId, sockConn);
			closesocket(sockConn);

			printf("���߳�%d: �߳��˳�\n", dwThreadId);
			return 0;
		}

		// ��ѹ���ݰ�
		printf("���߳�%d: ��ʼ��ѹ��Ϣ��...\n", dwThreadId);
		unsigned char debuffer[1024*1024];
		unsigned int long delen=1024*1024;
		int  err=tcper_gzuncompress(debuffer,&delen,recvbuffer,elen-1);

		if(err!=0)
		{
			printf("���߳�%d: ��ѹ���ݰ�����\n", dwThreadId);
			printf("���߳�%d: �رտͻ���Socket %d\n", dwThreadId, sockConn);
			closesocket(sockConn);

			printf("���߳�%d: �߳��˳�\n", dwThreadId);
			return 0;
		}

		printf("���߳�%d: ��ѹ��ϣ���С%d -> %d\n\n", dwThreadId, elen-1, delen);


		unsigned char sendbuffer[80*1024];
		int sendlen = 0;
		int sendcommand = 0;

		// ������ؽ��

		handle_msg(command, (char*)debuffer, delen, (char*)sendbuffer, sendlen, sendcommand, dwThreadId);

		BYTE *pack = NULL;
		int packlen = 0;
		pack = tcper_PackCmd(sendcommand, (BYTE*)sendbuffer, sendlen, packlen);
		printf("���߳�%d: ������ؽ��������%d\n", dwThreadId, packlen);

		// ���ʹ�����
		if(send(sockConn, (char*)pack, packlen, 0) == SOCKET_ERROR)
		{
			printf("���߳�%d: ���ʹ���������\n", dwThreadId);
			delete []pack;
			closesocket(sockConn);
			return 0;
		}

		delete []pack;
	}

	// �Ͽ����ӣ��ر�Socket
	closesocket(sockConn);
	return 1;
}