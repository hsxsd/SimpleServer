
// TradeServer_C.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTradeServer_CApp: 
// �йش����ʵ�֣������ TradeServer_C.cpp
//

class CTradeServer_CApp : public CWinApp
{
public:
	CTradeServer_CApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTradeServer_CApp theApp;