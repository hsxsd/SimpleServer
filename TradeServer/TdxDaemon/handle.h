#ifndef  _HANDLE_H_
#define  _HANDLE_H_

#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include "trader.h"
#include "UserManage.h"
//宏定义
#define BUY 15//登录
#define CANNAL_ORDER 26 //撤单
#define STOCK_DATA 14//行情----
#define VK_LOGIN 47//登录
#define VK_USER_FUNDS 48//资金
#define VK_ZIJINGUFEN 49//股份
#define VK_WEITUO   50//当天委托
#define VK_CHENGJIAO   51//当天成交
#define VK_CHE_DAN   52//撤单列表
#define VK_WEITUO_LISHI   53//委托历史
#define VK_CHENGJIO_LISHI   54//成交历史
#define VK_JIAOGEDAN   55//交割单
#define VK_ZIJINLIUSHUI   56//资金流水
#define VK_GUDONGLIST 57//股东列表
#define VC_RANK 58//市值列表

//类型定义
typedef unsigned char BYTE;
//结构体


struct UserInfo
{
	char UserName[30];
	char UserYinyebu[101];
	char UserNumA[101];
	char UserNumB[101];
	double FundsTotal;
	double LockFunds;
	double BeginFunds;
	char UserId[20];
	int ID;
	int status;//账号状态
	float SXFEE;
	float YHFEE;
	float GHFEE;
	float JSFEE;
};
struct RANK
{
	char UserId[20];
	char name[20];
	double beginfunds;
	double funds;
	double money;
	double allmoney;
};

struct TradeForm
{
	char HashCode[30];
	struct UserInfo UserInfo;
	char SockCode[10];
	char StockName[20];
	int Num;
	float Price;
	int TradeType;
	int TradeStatus;
	int reaPortType;
	int ID;
	double Time;
	float TrdePrice;
	int TrdeNum;
	double LastTime;
};
struct tagRCV_REPORT_STRUCTEx
{
	unsigned short m_cbSize;
	int m_time;
	unsigned short m_wMarket;
	char m_szLabel[10];
	char m_szName[32];
	float m_fLastClose;
	float m_fOpen;
	float m_fHigh;
	float m_fLow;
	float m_fNewPrice;
	float m_fVolume;
	float m_fAmount;
	float m_fBuyPrice[3];
	float m_fBuyVolume[3];
	float m_fSellPrice[3];
	float m_fSellVolume[3];
	float m_fBuyPrice4;
	float m_fBuyVolume4;
	float m_fSellPrice4;
	float m_fSellVolume4;
	float m_fBuyPrice5;
	float m_fBuyVolume5;
	float m_fSellPrice5;
	float m_fSellVolume5;
};
struct SockData
{
	struct tagRCV_REPORT_STRUCTEx Stock_data;
	char stockCode[10];
};

struct VC_ZIJINGUFEN
{
	char stockcode[10];
	char stockname[32];
	int num;
	int CansellNum;
	int xzltg;
	int wxltg;
	double buyPrice;
	double NewPrice;
	double NewShizhi;
	double FloatYk, Ykbl;
};

struct VC_UserFunds
{
	double TotalFunds;
	double LockFunds;
	double BeginFunds;
};
struct VC_WEITUO
{
	char stockcode[10];
	char stockname[32];
	int id;
	int creaTime;
	int trdeStatus;
	int trdeType;
	float price;
	float num;
	float turnoverPrice;
	float turnoverNum;
	int reaPortType;//0=限价 1=市价
};

struct VC_CHENGJIAO
{
	char stockcode[10];
	char stockname[32];
	int creatTime;
	int trdeType;
	int trdeStatus;
	int id;
	float turnoverPrice;
	float turnoverNum;
	float turnoverFunds;
};

struct VC_CHE_DAN
{
	char stockcode[10];
	char stockname[32];
	int time;
	int tradetag;//", typeof(string));
	int status;//", typeof(string));
	float trdeprice;//", typeof(decimal));
	float trdenum;//", typeof(int));
	int tradeid;//", typeof(string));
	float cjprice;//", typeof(decimal));
	float cjnum;//", typeof(float));
	int reporttype;//", typeof(string));
	int wtlb;//", typeof(string));
	int id;//", typeof(int));
	char hashCode[50];
};
struct Android_DateTimes
{
	short StartYear;
	BYTE StartMonth;
	BYTE StartDay;
	short EndYear;
	BYTE EndMonth;
	BYTE EndDay;
};

//委托历史
struct VC_WEITUO_LISHI
{
	char stockcode[10];
	char stockname[32];
	int Date;
	int Time;
	int trdeType;
	int trdeStatus;
	float Eprice;
	int Enum;
	int Eno;
	float Tprice;
	int Tnum;
	int id;
	int creaTime;
	float price;
	float num;
	float turnoverPrice;
	float turnoverNum;
	int reaPortType;//0=限价 1=市价
};

//成交历史
struct VC_CHENGJIO_LISHI
{
	char stockcode[10];
	char stockname[32];
	int Date;
	int Time;
	int trdeType;
	float Tprice;
	float Tnum;
	int Tno;
};
//资金流水
struct VC_ZIJINLIUSHUI
{
	char stockcode[10];
	char stockname[32];
	int Date;
	int Time;
	float price;
	int num;
	int busstype;
	double funds;
	double canusefunds;
};
//交割单
struct VC_JIAOGEDAN
{
	char stockcode[10];
	char stockname[32];
	int Date;
	int Time;
	float Tprice;
	float Tnum;
	int BussType;
	float TFunds;
	float Funds;
	float Sxfee,Ghfee,Yhfee,Qtfee;
	int Tno;
};

//外部调用全局变量

extern char *command_string[100];
//外部调用函数
void handle_init_command_string();
int handle_msg(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId,UserList *current_user);

#endif