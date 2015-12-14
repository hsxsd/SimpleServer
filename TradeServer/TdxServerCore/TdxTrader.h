#pragma once
#include <Windows.h>

typedef int(__cdecl *TDXINIT)();
typedef int(__cdecl *TDXEXIT)();
typedef int(__cdecl *TDXLOGOUT)();
typedef int(__cdecl *TDXSENDORDER)(int TradeType, int TradeMode, char *GdCode, char *StkCode, double Price, int volume, char *Result, char *ErrInfo);
typedef int(__cdecl *TDXQUERYDATA)(int, char *, char *);
typedef int(__cdecl *TDXGETQUOTE)(char*, char*, char*);
typedef int(__cdecl *TDXLOGIN)(char*, int, char*, char*, char *, char*);
typedef int(__cdecl *TDXQUERYHISTORY)(int, char*, char*, char*, char*);
typedef int(__cdecl *TDXCANCELORDER)(char *TradeID, int Mark, char *Result, char *ErrInfo);

class TdxTrader
{
private:
	HMODULE m_hDll;

	TDXINIT tdxinit;
	TDXLOGIN tdxlogin;
	TDXLOGOUT tdxlogout;
	TDXEXIT tdxexit;

	char Server[100];
	char Account[100];
	char JyPass[100];
	char TxPass[100];
	int port;
	char gddm1[100];
	char gddm2[100];
	char gdmc[100];


	bool LoadDll();
	bool LoadConfig();

public:
	TdxTrader();
	~TdxTrader();

	bool Init();
	TDXSENDORDER tdxsendorder;
	TDXQUERYDATA tdxquerydata;
	TDXQUERYHISTORY tdxqueryhistory;
	TDXGETQUOTE tdxgetquote;
	TDXCANCELORDER tdxcancelorder;
};