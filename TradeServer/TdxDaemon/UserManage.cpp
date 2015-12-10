#include "UserManage.h"

//用户链表
UserList User=NULL;

/*
初始化链表
*/
bool user_Init_List(UserList *User)
{	
	//printf("%d",sizeof(Node));
	*User = (UserList)malloc(sizeof(Node));
	if (!(*User))//存储分配失败
	{
		return false;
	}
	(*User)->next=NULL;
	return true;
}
/*
比较两个字符串是否相等
*/
bool user_strequal(char *s1,char *s2)
{
	if( s1 == NULL || s2 == NULL)
		return false;
	int len=strlen(s1);
	if( len != strlen(s2))
		return false;
	else{	
		int num=0;
		while(*s1 !='\0' && *s1++ ==*s2++) num++;
		if(len != num) return false;
	}
	return true;
}
/*
验证两个用户是否是同一个
*/
bool user_CompareInfo(login LoginUser1,login LoginUser2)
{
	if(&LoginUser1==NULL || &LoginUser2==NULL)
		 return false;
	if(user_strequal(LoginUser1.UserId,LoginUser2.UserId))
		printf("该用户名存在！\n");
	else {
		return false;
	}
	if(user_strequal(LoginUser1.UserPass,LoginUser2.UserPass))
		printf("密码正确,验证通过！\n");
	else {
		printf("密码输入有误！\n");
		return false;
	}

	return true;
	
}
/*
验证用户是否存在
*/
//int user_Find_User(UserList User,login LoginUser)
//{
//	printf("准备验证用户信息...");
//	UserList p=User->next;
//	int i=0,j=-1;
//	while(p){
//		if(user_CompareInfo((*p).userInfo,LoginUser))
//		{
//			j=i;
//			break;
//		}
//		i++;
//        p = p->next;
//	}
//	printf("用户信息验证完毕！");
//	return j;
//}
UserList user_Find_User(UserList User,login LoginUser)
{
	printf("准备验证用户信息...\n");
	UserList p=User->next;

	while(p){
		if(user_CompareInfo((*p).userInfo,LoginUser))
		{
			break;
		}
		p = p->next;
	}
	if(p==NULL)  printf("该用户不存在！\n");
	printf("用户信息验证完毕！\n\n");
	return p;
}
//UserList user_getUserName(int pos,UserList User)
//{
//	UserList p=User->next;
//	int i=0;
//	while(p){	
//		if(i==pos) break;
//		i++;
//		p=p->next;
//	}
//	return p;
//}
/*
添加用户
*/
bool user_AddUser(UserList User,int pos,login LoginUser)
{
	UserList p,q;
	
	p=User;
	int j=0;//链表没有头结点
	//寻找第pos个节点
	while (p && j<pos)//该节点存在
	{
		p=p->next;
		j++;
	}
	if (!p || j>pos)
		return false;
	q=(UserList)malloc(sizeof(Node));//为新加入的结点分配空间
	q->userInfo=LoginUser;
	q->next=p->next;//将p的后继结点赋给新添加的结点
	p->next=q;//将p的后继结点设为新结点

	return true;
	
}

/*
密码加密
*/
int md55(char *instring,char *output)
{
	int i;
	unsigned char encrypt[35];//="123456";//21232f297a57a5a743894a0e4a801fc3
	strcpy_s((char*)encrypt,35,instring);
	unsigned char decrypt[16];    
	MD5_CTX md5;
	MD5Init(&md5);         		
	MD5Update(&md5,encrypt,(unsigned int)strlen((char *)encrypt));
	MD5Final(&md5,decrypt);
	for(i=0;i<16;i++)
		sprintf(output+i*2,"%02x",decrypt[i]);
	return 1;
}
/*
初始化用户信息
*/
bool user_InitUser_List(char *accounts,UserList *User_List)
{
	printf("准备初始化用户链表...\n");
	if(!user_Init_List(User_List))
	{
		printf("初始化用户链表失败！\n");

		return false;
	}
	printf("初始化用户链表成功！\n\n");

	printf("准备获取用户信息...\n");

	FILE *fp;
	if(NULL == (fp=fopen(accounts,"r+")))
	{
		printf("打开文件失败，“accounts.txt”文件不存在！");
		printf("获取用户信息失败！\n");
		fclose(fp);
		return false;
	}

	login user;
	printf("用户信息：\n");
	char userPass[100];
	char str_txt[256];
	while(fgets(str_txt,sizeof(str_txt),fp) != NULL )
	{		
		if(str_txt[0] == ' ' || str_txt[0] == '\n' || str_txt[0] == '\t')
			continue;

		sscanf(str_txt,"%s %s",user.UserId,userPass);

		md55(userPass,user.UserPass);

		if(user_AddUser(*User_List,0,user))
			printf("用户 %s 插入链表成功！\n",user.UserId);
		else{
			printf("用户 %s 插入链表失败！\n");
			printf("获取用户信息失败！\n");
			fclose(fp);
			return false;
		}
	}

	//for(int i=0; i<UserNum;i++){
	//	fgets(str_txt,sizeof(str_txt),fp);
	//	char userPass[100];
	//	sscanf(str_txt,"%s %s",user.UserId,userPass);
	//	md55(userPass,user.UserPass);

	//	if(user_AddUser(*User_List,0,user))
	//		printf("用户 %s 插入链表成功！\n",user.UserId);
	//	else{
	//		printf("用户 %s 插入链表失败！\n");
	//		printf("获取用户信息失败！\n");
	//		fclose(fp);
	//		return false;
	//	}
	//}

	fclose(fp);
	printf("获取用户信息完毕！\n\n");
//<----------------------输出用户信息例程------------------------------------>
	//printf("输出用户信息：\n");
	//UserList p=(*User)->next;//链表表头无数据
	//while(p){
	//	printf("用户信息：%s %s\n",p->userInfo.UserId,p->userInfo.UserPass);
	//	p=p->next;
	//}
	return true;
}
/*
获取系统当前时间
*/
void user_current_time(char* current_date,char* current_time){
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	sprintf(current_date,"%4d%2d%2d",1900+timeinfo->tm_year,
		    1+timeinfo->tm_mon,timeinfo->tm_mday);
	sprintf(current_time,"%2d%2d%2d",timeinfo->tm_hour,
		    timeinfo->tm_min,timeinfo->tm_sec);
	printf("系统当前时间：%s %s\n",current_date,current_time);
}

bool user_Record(login LoginUser,char* TradeInfo)
{
	printf("准备保存委托记录...\n");
	FILE *fp;
	char filename[256];
	sprintf(filename,"%s%s",LoginUser.UserId,"_weituo.txt");
	fp = fopen(filename,"a+");//没有就建立，有就追加
	if(fp == NULL){
		printf("打开或建立记录文件失败！");
		return false;
	}
	fputs(TradeInfo,fp);
	fclose(fp);
	return true;
}
//void user_ConnMySql()
//{
//
//}
//bool user_SaveImage(login LoginUser,char* TradeInfo)
//{
//	
//}