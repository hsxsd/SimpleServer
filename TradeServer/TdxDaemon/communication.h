#ifndef  _COMMUNICATION_H_
#define  _COMMUNICATION_H_

#include <stdio.h>
#include <Winsock2.h>
#include "zlib.h"
#include <conio.h>
#include "handle.h"
#include "config.h"

#pragma comment (lib, "wsock32.lib")  
#pragma comment (lib, "zlib.lib")  

//结构体
struct RecvDataHeader
{
	char CheckSum;
	int Command;
	int Size;
};

struct ThreadData
{
	SOCKET sockConn;
};

//外部调用函数
int tcper_start();
#endif