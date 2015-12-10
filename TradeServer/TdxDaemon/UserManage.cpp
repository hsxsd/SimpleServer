#include "UserManage.h"

//�û�����
UserList User=NULL;

/*
��ʼ������
*/
bool user_Init_List(UserList *User)
{	
	//printf("%d",sizeof(Node));
	*User = (UserList)malloc(sizeof(Node));
	if (!(*User))//�洢����ʧ��
	{
		return false;
	}
	(*User)->next=NULL;
	return true;
}
/*
�Ƚ������ַ����Ƿ����
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
��֤�����û��Ƿ���ͬһ��
*/
bool user_CompareInfo(login LoginUser1,login LoginUser2)
{
	if(&LoginUser1==NULL || &LoginUser2==NULL)
		 return false;
	if(user_strequal(LoginUser1.UserId,LoginUser2.UserId))
		printf("���û������ڣ�\n");
	else {
		return false;
	}
	if(user_strequal(LoginUser1.UserPass,LoginUser2.UserPass))
		printf("������ȷ,��֤ͨ����\n");
	else {
		printf("������������\n");
		return false;
	}

	return true;
	
}
/*
��֤�û��Ƿ����
*/
//int user_Find_User(UserList User,login LoginUser)
//{
//	printf("׼����֤�û���Ϣ...");
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
//	printf("�û���Ϣ��֤��ϣ�");
//	return j;
//}
UserList user_Find_User(UserList User,login LoginUser)
{
	printf("׼����֤�û���Ϣ...\n");
	UserList p=User->next;

	while(p){
		if(user_CompareInfo((*p).userInfo,LoginUser))
		{
			break;
		}
		p = p->next;
	}
	if(p==NULL)  printf("���û������ڣ�\n");
	printf("�û���Ϣ��֤��ϣ�\n\n");
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
����û�
*/
bool user_AddUser(UserList User,int pos,login LoginUser)
{
	UserList p,q;
	
	p=User;
	int j=0;//����û��ͷ���
	//Ѱ�ҵ�pos���ڵ�
	while (p && j<pos)//�ýڵ����
	{
		p=p->next;
		j++;
	}
	if (!p || j>pos)
		return false;
	q=(UserList)malloc(sizeof(Node));//Ϊ�¼���Ľ�����ռ�
	q->userInfo=LoginUser;
	q->next=p->next;//��p�ĺ�̽�㸳������ӵĽ��
	p->next=q;//��p�ĺ�̽����Ϊ�½��

	return true;
	
}

/*
�������
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
��ʼ���û���Ϣ
*/
bool user_InitUser_List(char *accounts,UserList *User_List)
{
	printf("׼����ʼ���û�����...\n");
	if(!user_Init_List(User_List))
	{
		printf("��ʼ���û�����ʧ�ܣ�\n");

		return false;
	}
	printf("��ʼ���û�����ɹ���\n\n");

	printf("׼����ȡ�û���Ϣ...\n");

	FILE *fp;
	if(NULL == (fp=fopen(accounts,"r+")))
	{
		printf("���ļ�ʧ�ܣ���accounts.txt���ļ������ڣ�");
		printf("��ȡ�û���Ϣʧ�ܣ�\n");
		fclose(fp);
		return false;
	}

	login user;
	printf("�û���Ϣ��\n");
	char userPass[100];
	char str_txt[256];
	while(fgets(str_txt,sizeof(str_txt),fp) != NULL )
	{		
		if(str_txt[0] == ' ' || str_txt[0] == '\n' || str_txt[0] == '\t')
			continue;

		sscanf(str_txt,"%s %s",user.UserId,userPass);

		md55(userPass,user.UserPass);

		if(user_AddUser(*User_List,0,user))
			printf("�û� %s ��������ɹ���\n",user.UserId);
		else{
			printf("�û� %s ��������ʧ�ܣ�\n");
			printf("��ȡ�û���Ϣʧ�ܣ�\n");
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
	//		printf("�û� %s ��������ɹ���\n",user.UserId);
	//	else{
	//		printf("�û� %s ��������ʧ�ܣ�\n");
	//		printf("��ȡ�û���Ϣʧ�ܣ�\n");
	//		fclose(fp);
	//		return false;
	//	}
	//}

	fclose(fp);
	printf("��ȡ�û���Ϣ��ϣ�\n\n");
//<----------------------����û���Ϣ����------------------------------------>
	//printf("����û���Ϣ��\n");
	//UserList p=(*User)->next;//�����ͷ������
	//while(p){
	//	printf("�û���Ϣ��%s %s\n",p->userInfo.UserId,p->userInfo.UserPass);
	//	p=p->next;
	//}
	return true;
}
/*
��ȡϵͳ��ǰʱ��
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
	printf("ϵͳ��ǰʱ�䣺%s %s\n",current_date,current_time);
}

bool user_Record(login LoginUser,char* TradeInfo)
{
	printf("׼������ί�м�¼...\n");
	FILE *fp;
	char filename[256];
	sprintf(filename,"%s%s",LoginUser.UserId,"_weituo.txt");
	fp = fopen(filename,"a+");//û�оͽ������о�׷��
	if(fp == NULL){
		printf("�򿪻�����¼�ļ�ʧ�ܣ�");
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