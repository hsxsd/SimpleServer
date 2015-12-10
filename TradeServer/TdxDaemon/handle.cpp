#include "handle.h"
#include "config.h"

//全局变量

char *marks[2]={ "深圳A股", "上海A股"};
char *tradetype[8]={ "买入", "卖出", "撤买入", "撤卖出", "银行转存", "银行转取", "股本分红", "转增股本"};
char *tradestatus[4]={"未成交", "已成","已撤买","已撤卖"};
char *reaPortType[2]={"限价","市价"};

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
	printf("子线程%d: 处理登录指令\n", dwThreadId);
	
}

void handle_buy(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 处理下单指令\n", dwThreadId);
	
}

void handle_userfunds(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到查询资金指令\n", dwThreadId);

	
}


void handle_zijingufen(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到查询股份指令\n", dwThreadId);

	
}

void handle_weituo(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到查询当日委托指令\n", dwThreadId);

	
}
void handle_chengjiao(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到查询当日委托指令\n", dwThreadId);

	
}
void handle_chedan(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到查询撤单委托指令\n", dwThreadId);

	
}
void handle_chaxun5d(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到查询股票五档报价指令\n", dwThreadId);

	
}
void handle_weituolishi(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到委托历史查询指令\n", dwThreadId);

	
}
void handle_chengjiaolishi(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到成交历史查询指令\n", dwThreadId);

	
}
void handle_zijinliushui(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到资金历史查询指令\n", dwThreadId);

	
}
void handle_jiaogedan(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到交割单历史查询指令\n", dwThreadId);

	
}

void handle_chedanorder(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到撤单指令\n", dwThreadId);
	
}
int handle_msg(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId)
{
	char *Result = new char[10 * 1024 * 1024];
	char *ErrInfo = new char[1024 * 1024];

	// 处理消息
	printf("子线程%d: 开始处理消息...\n\n", dwThreadId);
	if (command == VK_LOGIN)//登录时需要验证信息
	{
		handle_login(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo);
	}
	else if (command == BUY)//下单成功时需要记录信息
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
	else if (command == VK_WEITUO)//委托信息
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
		printf("子线程%d: 接收到查询VC_RANK指令\n", dwThreadId);
		strcpy((char*)sendbuffer, "实盘不支持的查询指令");
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
		strcpy((char*)sendbuffer, "你好，TCP客户端，我是服务端!\n");
		printf("子线程%d: 返回请求结果：%s\n", dwThreadId, sendbuffer);
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 0;
	}

	delete[]Result;
	delete[]ErrInfo;

	return 1;
}