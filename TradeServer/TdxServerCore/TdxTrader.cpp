#include "stdafx.h"
#include "TdxTrader.h"
#include <stdio.h>

TdxTrader::TdxTrader()
{
	m_hDll = NULL;

	tdxinit = NULL;
	tdxlogin = NULL;
	tdxlogout = NULL;
	tdxexit = NULL;
	tdxsendorder = NULL;
	tdxquerydata = NULL;
	tdxqueryhistory = NULL;
	tdxgetquote = NULL;
	tdxcancelorder = NULL;
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
	if (!LoadDll())
	{
		printf("����DLLʧ��\n");
		return false;
	}
	
	printf("DLL�������\n");

	printf("׼����ʼ��ʵ�̽ӿ�...\n");
	if (tdxinit() != 1)
	{
		printf("��ʼ��ʵ�̽ӿ�ʧ�ܣ�\n");
		return false;
	}

	printf("ʵ�̽ӿڳ�ʼ����ϣ�\n");

	if (!LoadConfig())
	{
		printf("���������ļ�ʧ��\n");
		return false;
	}

	printf("ʵ��IP: %s\n�˿ڣ�%d\n�˺ţ�%s\n", Server, port, Account);

	printf("�����뽻�����룺");
	scanf_s("%s", JyPass, 100);

	char *ErrInfo = new char[1024 * 1024];
	if (tdxlogin(Server, port, Account, JyPass, TxPass, ErrInfo) != 1)
	{
		printf("��¼ʵ�̽ӿ�ʧ�ܣ�%s\n", ErrInfo);
		delete ErrInfo;

		printf("�ͷ�ʵ�̽ӿ�\n");
		tdxexit();//�ͷŽӿ�

		printf("�ͷ�tdxtrade.dll\n");
		FreeLibrary(m_hDll);

		return false;
	}

	delete ErrInfo;

	printf("ʵ�̽ӿڵ�¼��ϣ�\n\n");

	// ���ùɶ�����͹ɶ�����
	printf("�ɶ�����1=%s���ɶ�����2=%s���ɶ�����=%s\n", gddm1, gddm2, gdmc);

	return true;
}

bool TdxTrader::LoadDll()
{
	if (m_hDll != NULL)
		return false;

	m_hDll = LoadLibrary("tdxtrade.dll");
	if (m_hDll == NULL)
		return false;

	tdxlogout = (TDXLOGOUT)GetProcAddress(m_hDll, "TdxLogout");
	tdxinit = (TDXINIT)GetProcAddress(m_hDll, "TdxInit");
	tdxlogin = (TDXLOGIN)GetProcAddress(m_hDll, "TdxLogin");
	tdxgetquote = (TDXGETQUOTE)GetProcAddress(m_hDll, "TdxGetQuote");
	tdxsendorder = (TDXSENDORDER)GetProcAddress(m_hDll, "TdxSendOrder");
	tdxqueryhistory = (TDXQUERYHISTORY)GetProcAddress(m_hDll, "TdxQueryHistory");
	tdxquerydata = (TDXQUERYDATA)GetProcAddress(m_hDll, "TdxQueryData");
	tdxexit = (TDXEXIT)GetProcAddress(m_hDll, "TdxExit");
	tdxcancelorder = (TDXCANCELORDER)GetProcAddress(m_hDll, "TdxCancelOrder");

	if (tdxinit == NULL || tdxlogin == NULL || tdxqueryhistory == NULL ||
		tdxlogout == NULL || tdxgetquote == NULL || tdxquerydata == NULL || tdxexit == NULL)
		return false;

	return true;
}

bool TdxTrader::LoadConfig()
{
	char path[200];
	GetCurrentDirectory(256, path);//��������ļ�����Ŀ¼
	char configpath[250];
	int namelen = 100;
	sprintf_s(configpath, "%s%s%s", path, "\\", "config.ini");
	GetPrivateProfileString("config", "gddm1", "", gddm1, namelen, configpath); //û�и�ֵʱĬ��Ϊ��
	GetPrivateProfileString("config", "gddm2", "", gddm2, namelen, configpath);
	GetPrivateProfileString("config", "gdmc", "", gdmc, namelen, configpath);
	GetPrivateProfileString("config", "ip", "", Server, namelen, configpath);
	GetPrivateProfileString("config", "account", "", Account, namelen, configpath);
	GetPrivateProfileString("config", "jypass", "", JyPass, namelen, configpath);
	GetPrivateProfileString("config", "txpass", "", TxPass, namelen, configpath);
	port = (int)GetPrivateProfileInt("config", "port", 0, configpath);

	return true;
}



