#include "handle.h"
#include "config.h"

//ȫ�ֱ���

char *marks[2]={ "����A��", "�Ϻ�A��"};
char *tradetype[8]={ "����", "����", "������", "������", "����ת��", "����תȡ", "�ɱ��ֺ�", "ת���ɱ�"};
char *tradestatus[4]={"δ�ɽ�", "�ѳ�","�ѳ���","�ѳ���"};
char *reaPortType[2]={"�޼�","�м�"};

char *command_string[100];

void handle_init_command_string()
{
	command_string[BUY] = "BUY";
	command_string[CANNAL_ORDER] = "CANNAL_ORDER";
	command_string[STOCK_DATA] = "STOCK_DATA";
	command_string[VK_LOGIN] = "VK_LOGIN";
	command_string[VK_USER_FUNDS] = "VK_USER_FUNDS";
	command_string[VK_ZIJINGUFEN] = "VK_ZIJINGUFEN";
	command_string[VK_WEITUO] = "VK_WEITUO";
	command_string[VK_CHENGJIAO] = "VK_CHENGJIAO";
	command_string[VK_CHE_DAN] = "VK_CHE_DAN";
	command_string[VK_WEITUO_LISHI] = "VK_WEITUO_LISHI";
	command_string[VK_CHENGJIO_LISHI] = "VK_CHENGJIO_LISHI";
	command_string[VK_JIAOGEDAN] = "VK_JIAOGEDAN";
	command_string[VK_ZIJINLIUSHUI] = "VK_ZIJINLIUSHUI";
	command_string[VK_GUDONGLIST] = "VK_GUDONGLIST";
	command_string[VC_RANK] = "VC_RANK";
}

void handle_login(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: �����¼ָ��\n", dwThreadId);
	
}

void handle_buy(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: �����µ�ָ��\n", dwThreadId);
	
}

void handle_userfunds(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ���ѯ�ʽ�ָ��\n", dwThreadId);

	
}


void handle_zijingufen(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ���ѯ�ɷ�ָ��\n", dwThreadId);

	
}

void handle_weituo(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ���ѯ����ί��ָ��\n", dwThreadId);

	
}
void handle_chengjiao(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ���ѯ����ί��ָ��\n", dwThreadId);

	
}
void handle_chedan(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ���ѯ����ί��ָ��\n", dwThreadId);

	
}
void handle_chaxun5d(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ���ѯ��Ʊ�嵵����ָ��\n", dwThreadId);

	
}
void handle_weituolishi(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ�ί����ʷ��ѯָ��\n", dwThreadId);

	
}
void handle_chengjiaolishi(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ��ɽ���ʷ��ѯָ��\n", dwThreadId);

	
}
void handle_zijinliushui(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ��ʽ���ʷ��ѯָ��\n", dwThreadId);

	
}
void handle_jiaogedan(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ������ʷ��ѯָ��\n", dwThreadId);

	
}

void handle_chedanorder(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ�����ָ��\n", dwThreadId);
	
}
int handle_msg(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId)
{
	char *Result = new char[10 * 1024 * 1024];
	char *ErrInfo = new char[1024 * 1024];

	// ������Ϣ
	printf("���߳�%d: ��ʼ������Ϣ...\n\n", dwThreadId);
	if (command == VK_LOGIN)//��¼ʱ��Ҫ��֤��Ϣ
	{
		handle_login(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo);
	}
	else if (command == BUY)//�µ��ɹ�ʱ��Ҫ��¼��Ϣ
	{
		handle_buy(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo);
	}
	else if (command == VK_USER_FUNDS)
	{
		handle_userfunds(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo);
	}
	else if (command == VK_ZIJINGUFEN)
	{
		handle_zijingufen(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo);
	}
	else if (command == VK_WEITUO)//ί����Ϣ
	{
		handle_weituo(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo);
	}
	else if (command == VK_CHENGJIAO)
	{
		handle_chengjiao(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo);
	}
	else if (command == VK_CHE_DAN)
	{
		handle_chedan(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo);
	}
	else if (command == VC_RANK)
	{
		printf("���߳�%d: ���յ���ѯVC_RANKָ��\n", dwThreadId);
		strcpy((char*)sendbuffer, "ʵ�̲�֧�ֵĲ�ѯָ��");
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 1;
	}
	else if (command == STOCK_DATA)
	{
		handle_chaxun5d(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo);
	}
	else if (command == VK_WEITUO_LISHI)
	{
		handle_weituolishi(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo);
	}
	else if (command == VK_CHENGJIO_LISHI)
	{
		handle_chengjiaolishi(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo);
	}
	else if (command == VK_ZIJINLIUSHUI)
	{
		handle_zijinliushui(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo);
	}
	else if (command == VK_JIAOGEDAN)
	{
		handle_jiaogedan(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo);
	}
	else if (command == CANNAL_ORDER)
	{
		handle_chedanorder(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo);
	}
	else
	{
		strcpy((char*)sendbuffer, "��ã�TCP�ͻ��ˣ����Ƿ����!\n");
		printf("���߳�%d: ������������%s\n", dwThreadId, sendbuffer);
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 0;
	}

	delete[]Result;
	delete[]ErrInfo;

	return 1;
}