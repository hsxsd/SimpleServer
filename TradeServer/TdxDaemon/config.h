#ifndef  _CONFIG_H_
#define  _CONFIG_H_
#include<stdio.h>
#include<stdio.h>
#include<Windows.h>
#include <winnt.h>


void get_config(char *gddm1, char *gddm2, char *gdmc, char *server_ip, char *account, char *JyPass, char *TxPass, int *port);
char *get_config(char *lpKeyName,const int len,char* lpAppName);
#endif