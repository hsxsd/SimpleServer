#include "trader.h"
#include <stdio.h>
#include "config.h"

static TDXINIT tdxinit = NULL;
static TDXLOGIN tdxlogin = NULL;
static TDXLOGOUT tdxlogout = NULL;
static TDXEXIT tdxexit = NULL;

static HMODULE hDLL = NULL;



TDXSENDORDER tdxsendorder = NULL;
TDXQUERYDATA tdxquerydata = NULL;
TDXQUERYHISTORY tdxqueryhistory = NULL;
TDXGETQUOTE tdxgetquote = NULL;
TDXCANCELORDER tdxcancelorder = NULL;

int trader_init()
{
	// 加载tdxtrade.dll
	printf("准备加载tdxtrade.dll...\n");

	hDLL = LoadLibrary("tdxtrade.dll");

	if ( hDLL == NULL){
		printf("tdxtrade.dll加载失败！");
		return 0;
	}
	printf("加载tdxtrade.dll完毕！\n\n");

	//获取加载库函数地址
	printf("准备获取dll函数入口...\n");

	tdxlogout = (TDXLOGOUT)GetProcAddress(hDLL,"TdxLogout");
	tdxinit = (TDXINIT)GetProcAddress(hDLL,"TdxInit");
	tdxlogin = (TDXLOGIN)GetProcAddress(hDLL,"TdxLogin");
	tdxgetquote = (TDXGETQUOTE)GetProcAddress(hDLL,"TdxGetQuote");
	tdxsendorder = (TDXSENDORDER)GetProcAddress(hDLL,"TdxSendOrder");
	tdxqueryhistory = (TDXQUERYHISTORY)GetProcAddress(hDLL,"TdxQueryHistory");
	tdxquerydata = (TDXQUERYDATA)GetProcAddress(hDLL,"TdxQueryData");
	tdxexit =(TDXEXIT)GetProcAddress(hDLL,"TdxExit");
	tdxcancelorder = (TDXCANCELORDER)GetProcAddress(hDLL,"TdxCancelOrder");

	bool FunAddress = tdxinit==NULL || tdxlogin==NULL || tdxqueryhistory==NULL ||
		tdxlogout==NULL || tdxgetquote==NULL || tdxquerydata==NULL || tdxexit==NULL;

	if(FunAddress){
		printf("获取函数地址失败！\n");
		printf("释放tdxtrade.dll！\n");
		FreeLibrary(hDLL);
		return 0;
	}

	printf("获取dll函数入口完毕！\n\n");


	// 初始化实盘接口
	printf("准备初始化实盘接口...\n");
	int res = tdxinit();
	if (res != 1)
	{
		printf("初始化实盘接口失败！\n");
		printf("释放tdxtrade.dll！\n");
		FreeLibrary(hDLL);
		return 0;
	}
	printf("实盘接口初始化完毕！\n\n");


	// 登录实盘接口
	printf("准备登录实盘接口...\n");	

	char Server[100];
	char Account[100];
	char JyPass[100];
	char TxPass[100];
	int port;
	char gddm1[100];
	char gddm2[100];
	char gdmc[100];

	get_config(gddm1,gddm2,gdmc,Server,Account,JyPass,TxPass,&port);
	char *ErrInfo = (char*)malloc(sizeof(char) * 1024 * 1024);
	printf("实盘IP: %s\n端口：%d\n账号：%s\n", Server, port, Account);
	
	printf("请输入交易密码：");
	scanf("%s", JyPass);

	res = tdxlogin(Server,port,Account,JyPass,TxPass,ErrInfo);
	if (res != 1)
	{
		printf("登录实盘接口失败：%d %s\n", res, ErrInfo);
		free(ErrInfo);

		printf("释放实盘接口\n");
		tdxexit();//释放接口

		printf("释放tdxtrade.dll\n");
		FreeLibrary(hDLL);

		return 0;
	}

	free(ErrInfo);
	printf("实盘接口登录完毕！\n\n");

	// 配置股东代码和股东名称
	printf("股东代码1=%s，股东代码2=%s，股东名称=%s\n", gddm1, gddm2, gdmc);
	printf("\n");

	return 1;
}

void trader_exit()
{
	printf("释放实盘接口\n");
	tdxexit();//释放接口

	printf("释放tdxtrade.dll\n");
	FreeLibrary(hDLL);
}