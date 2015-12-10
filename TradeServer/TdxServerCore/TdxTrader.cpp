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
		printf("ж��Dll\n");
		FreeLibrary(m_hDll);
	}
}


bool TdxTrader::Init()
{
	m_hDll = LoadLibrary("tdxtrade.dll");
	if (m_hDll == NULL)
	{
		printf("����Dllʧ��\n");
		return false;
	}
	else
	{
		printf("����Dll�ɹ�\n");
		return true;
	}
}