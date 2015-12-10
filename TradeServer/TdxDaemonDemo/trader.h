#pragma once

#include <Winsock2.h>

// �ⲿ�õ��Ľṹ��/���Ͷ���
typedef int(__cdecl *TDXINIT)();
typedef int(__cdecl *TDXEXIT)();
typedef int(__cdecl *TDXLOGOUT)();
typedef int(__cdecl *TDXSENDORDER)(int TradeType, int TradeMode, char *GdCode, char *StkCode, double Price, int volume, char *Result, char *ErrInfo);
typedef int(__cdecl *TDXQUERYDATA)(int, char *, char *);
typedef int(__cdecl *TDXGETQUOTE)(char*, char*, char*);
typedef int(__cdecl *TDXLOGIN)(char*, int, char*, char*, char *, char*);
typedef int(__cdecl *TDXQUERYHISTORY)(int, char*, char*, char*, char*);
typedef int(__cdecl *TDXCANCELORDER)(char *TradeID, int Mark, char *Result, char *ErrInfo);


// �ⲿ���õ�ȫ�ֱ���
extern char gddm1[100];
extern char gddm2[100];
extern char gdmc[100];

extern TDXSENDORDER tdxsendorder;
extern TDXQUERYDATA tdxquerydata;
extern TDXQUERYHISTORY tdxqueryhistory;
extern TDXGETQUOTE tdxgetquote;
extern TDXCANCELORDER tdxcancelorder;

// �ⲿ���õĺ���
int trader_init();
void trader_exit();
