
// TradeServer_C.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CTradeServer_CApp: 
// 有关此类的实现，请参阅 TradeServer_C.cpp
//

class CTradeServer_CApp : public CWinApp
{
public:
	CTradeServer_CApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CTradeServer_CApp theApp;