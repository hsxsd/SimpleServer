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
		printf("卸载Dll\n");
		FreeLibrary(m_hDll);
	}
}


bool TdxTrader::Init()
{
	if (!LoadDll())
	{
		printf("加载DLL失败\n");
		return false;
	}
	
	printf("DLL加载完毕\n");

	printf("准备初始化实盘接口...\n");
	if (tdxinit() != 1)
	{
		printf("初始化实盘接口失败！\n");
		return false;
	}

	printf("实盘接口初始化完毕！\n");

	if (!LoadConfig())
	{
		printf("加载配置文件失败\n");
		return false;
	}

	printf("实盘IP: %s\n端口：%d\n账号：%s\n", Server, port, Account);

	printf("请输入交易密码：");
	scanf_s("%s", JyPass, 100);

	char *ErrInfo = new char[1024 * 1024];
	if (tdxlogin(Server, port, Account, JyPass, TxPass, ErrInfo) != 1)
	{
		printf("登录实盘接口失败：%s\n", ErrInfo);
		delete ErrInfo;

		printf("释放实盘接口\n");
		tdxexit();//释放接口

		printf("释放tdxtrade.dll\n");
		FreeLibrary(m_hDll);

		return false;
	}

	delete ErrInfo;

	printf("实盘接口登录完毕！\n\n");

	// 配置股东代码和股东名称
	printf("股东代码1=%s，股东代码2=%s，股东名称=%s\n", gddm1, gddm2, gdmc);

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
	GetCurrentDirectory(256, path);//获得配置文件绝对目录
	char configpath[250];
	int namelen = 100;
	sprintf_s(configpath, "%s%s%s", path, "\\", "config.ini");
	GetPrivateProfileString("config", "gddm1", "", gddm1, namelen, configpath); //没有该值时默认为空
	GetPrivateProfileString("config", "gddm2", "", gddm2, namelen, configpath);
	GetPrivateProfileString("config", "gdmc", "", gdmc, namelen, configpath);
	GetPrivateProfileString("config", "ip", "", Server, namelen, configpath);
	GetPrivateProfileString("config", "account", "", Account, namelen, configpath);
	GetPrivateProfileString("config", "jypass", "", JyPass, namelen, configpath);
	GetPrivateProfileString("config", "txpass", "", TxPass, namelen, configpath);
	port = (int)GetPrivateProfileInt("config", "port", 0, configpath);

	return true;
}



