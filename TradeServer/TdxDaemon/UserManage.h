#ifndef  _USERMANAGE_H_
#define  _USERMANAGE_H_

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <time.h>
#include "md5.h"


//�ṹ��
struct login
{
	char UserId[35];
	char UserPass[35];
};

typedef struct Node 
{
	login userInfo;
	struct Node *next; 	
}Node;
typedef struct Node *UserList;

extern UserList User;

//�ⲿ���ú���
bool user_InitUser_List(char *accounts,UserList *User);
bool user_Record(login LoginUser,char* TradeInfo);
UserList user_Find_User(UserList User,login LoginUser);
void user_current_time(char* cruuent_date,char* current_time);
//UserList user_getUserName(int pos,UserList User);
#endif