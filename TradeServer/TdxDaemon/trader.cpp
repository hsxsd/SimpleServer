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
	// ����tdxtrade.dll
	printf("׼������tdxtrade.dll...\n");

	hDLL = LoadLibrary("tdxtrade.dll");

	if ( hDLL == NULL){
		printf("tdxtrade.dll����ʧ�ܣ�");
		return 0;
	}
	printf("����tdxtrade.dll��ϣ�\n\n");

	//��ȡ���ؿ⺯����ַ
	printf("׼����ȡdll�������...\n");

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
		printf("��ȡ������ַʧ�ܣ�\n");
		printf("�ͷ�tdxtrade.dll��\n");
		FreeLibrary(hDLL);
		return 0;
	}

	printf("��ȡdll���������ϣ�\n\n");


	// ��ʼ��ʵ�̽ӿ�
	printf("׼����ʼ��ʵ�̽ӿ�...\n");
	int res = tdxinit();
	if (res != 1)
	{
		printf("��ʼ��ʵ�̽ӿ�ʧ�ܣ�\n");
		printf("�ͷ�tdxtrade.dll��\n");
		FreeLibrary(hDLL);
		return 0;
	}
	printf("ʵ�̽ӿڳ�ʼ����ϣ�\n\n");


	// ��¼ʵ�̽ӿ�
	printf("׼����¼ʵ�̽ӿ�...\n");	

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
	printf("ʵ��IP: %s\n�˿ڣ�%d\n�˺ţ�%s\n", Server, port, Account);
	
	printf("�����뽻�����룺");
	scanf("%s", JyPass);

	res = tdxlogin(Server,port,Account,JyPass,TxPass,ErrInfo);
	if (res != 1)
	{
		printf("��¼ʵ�̽ӿ�ʧ�ܣ�%d %s\n", res, ErrInfo);
		free(ErrInfo);

		printf("�ͷ�ʵ�̽ӿ�\n");
		tdxexit();//�ͷŽӿ�

		printf("�ͷ�tdxtrade.dll\n");
		FreeLibrary(hDLL);

		return 0;
	}

	free(ErrInfo);
	printf("ʵ�̽ӿڵ�¼��ϣ�\n\n");

	// ���ùɶ�����͹ɶ�����
	printf("�ɶ�����1=%s���ɶ�����2=%s���ɶ�����=%s\n", gddm1, gddm2, gdmc);
	printf("\n");

	return 1;
}

void trader_exit()
{
	printf("�ͷ�ʵ�̽ӿ�\n");
	tdxexit();//�ͷŽӿ�

	printf("�ͷ�tdxtrade.dll\n");
	FreeLibrary(hDLL);
}