#include "stdafx.h"
#include "TdxTrader.h"
#include <stdio.h>

TdxTrader::TdxTrader()
{
	m_hDll = NULL;
}

TdxTrader::~TdxTrader()
{
	if (m_hDll != NULL)
	{
		printf("卸载Dll\n");
		FreeLibrary(m_hDll);
	}
}


bool TdxTrader::Init()
{
	m_hDll = LoadLibrary("tdxtrade.dll");
	if (m_hDll == NULL)
	{
		printf("加载Dll失败\n");
		return false;
	}
	else
	{
		printf("加载Dll成功\n");
		return true;
	}
}