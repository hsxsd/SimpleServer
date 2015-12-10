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

void handle_login(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo,UserList *current_user)
{
	printf("���߳�%d: �����¼ָ��\n", dwThreadId);
	if(delen == sizeof(login))
	{
		login *lg = (login*)debuffer;
		printf("���߳�%d: �û�����%s, ���룺%s\n", dwThreadId, lg->UserId, lg->UserPass);
		//��֤�û���Ϣ
		*current_user=user_Find_User(User,*lg);
		printf("��ǰ��¼�û���%s \n",(**current_user).userInfo.UserId);
		// �����û���Ϣ
		struct UserInfo info;
		char *gddm1;
		char *gddm2;
		char *gdmc;
		gddm1=get_config("gddm1",100,"config");
		gddm2=get_config("gddm2",100,"config");
		gdmc=get_config("gdmc",100,"config");
		strcpy(info.UserName, gdmc);
		strcpy(info.UserYinyebu, "test");
		strcpy(info.UserNumA, gddm1);
		strcpy(info.UserNumB, gddm2);
		strcpy(info.UserId, "test");
		delete [] gddm1;
		delete [] gddm2;
		delete [] gdmc;
		if(*current_user==NULL)
			info.ID = -1;
		else
			info.ID = 1;
		info.status = 1;
		info.SXFEE = info.YHFEE = info.GHFEE = info.JSFEE = 0;

		// ��ѯ�û��ʽ���Ϣ
		int ret = tdxquerydata(0, Result, ErrInfo);

		if (ret == 1)
		{
			int width = 8;
			int rows = 1;
			int cols = 0;
			int wordlen = 0;
			char word[100];
			printf("��1�У�");
			for (unsigned long i = 0; i <= strlen(Result); i++)
			{
				// ���һ���ַ�
				char nextchar = Result[i];

				if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
				{
					// ǰһ���ַ�������
					cols++;
					word[wordlen] = 0;

					wordlen = 0;

					if (rows == 2 && cols == 3) // �����ʽ�
					{							
						info.FundsTotal = atof(word);
					}
					if (rows == 2 && cols == 4) // �����ʽ�
					{
						info.LockFunds = atof(word);
					}
					if (rows == 2 && cols == 6) // ���ʲ�
					{
						info.BeginFunds = atof(word); // ģ�������ʹ�ÿ�ʼ�ʽ�
					}
				}

				if (nextchar == '\n')
				{
					// ����
					rows++;
					cols = 0;

				}
				else if (nextchar != '\t' && nextchar != 0)
				{
					word[wordlen] = nextchar;
					wordlen++;		
				}
			}					
		}
		else
		{
			info.FundsTotal = info.LockFunds = info.BeginFunds = 0;
		}


		// ��䷢�ͻ�����
		sendlen = sizeof(UserInfo);
		memcpy(sendbuffer, (char*)&info, sendlen);
		sendcommand = VK_LOGIN;
		printf("��䷢�ͻ���������С%d\n", sendlen);
	}
}

void handle_buy(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo,UserList current_user)
{
	printf("���߳�%d: �����µ�ָ��\n", dwThreadId);
	if(delen == sizeof(TradeForm))
	{
		struct TradeForm *tf = (struct TradeForm*)debuffer;
		printf("���߳�%d: TradeType %s, TradeMode %s, Price %.2f\n", dwThreadId, tradetype[tf->TradeType], reaPortType[tf->reaPortType], tf->Price);

		// ��ʼ�µ�
		int ret = tdxsendorder(tf->TradeType, tf->reaPortType, "", tf->SockCode, tf->Price, tf->Num, Result, ErrInfo);

		//����
		//int ret=1;
		if (ret == 1)//�µ��ɹ�
		{
			char tmp1[10], tmp2[10];
			char tradeid[10], mark[10];
			sscanf(Result, "%s%s%s%s", tmp1, tmp2, tradeid, mark);

			char return_str[20];
			sprintf(return_str, "%s\t%s", tradeid, mark);

			//��¼�û��µ���Ϣ
			char current_t[20],current_d[20];//tradeid�µ����
			user_current_time(current_d,current_t);
			char TradeInfo[512];
			//sprintf(TradeInfo,"%s%s %s %s %s %d","\n",current_d,current_t,"909090",tradetype[tf->TradeType],tf->Num);
			sprintf(TradeInfo,"%s%s %s %s %s %d","\n",current_d,current_t,tradeid,tradetype[tf->TradeType],tf->Num);
			user_Record(current_user->userInfo,TradeInfo);

			// ��䷢�ͻ�����
			sendlen = strlen(return_str) + 1;
			memcpy(sendbuffer, return_str, sendlen);
			sendcommand = 1;
		}
		else
		{
			strcpy((char*)sendbuffer, "�µ�ʧ��");
			sendlen = strlen((char*)sendbuffer) + 1;
			sendcommand = 0;
		}
	}
}

void handle_userfunds(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ���ѯ�ʽ�ָ��\n", dwThreadId);

	int ret = tdxquerydata(0, Result, ErrInfo);

	if (ret == 1)
	{
		struct VC_UserFunds uf;

		int rows = 1;
		int cols = 0;
		int wordlen = 0;
		char word[100];
		for (unsigned long i = 0; i <= strlen(Result); i++)
		{
			// ���һ���ַ�
			char nextchar = Result[i];

			if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
			{
				// ǰһ���ַ�������
				cols++;
				word[wordlen] = 0;

				wordlen = 0;

				if (rows == 2 && cols == 3) // �����ʽ�
				{							
					uf.TotalFunds = atof(word);
					printf("�����ʽ�%f ", atof(word));
				}
				if (rows == 2 && cols == 4) // �����ʽ�
				{
					uf.LockFunds = atof(word);
					printf("�����ʽ�%f ", atof(word));
				}
				if (rows == 2 && cols == 6) // ���ʲ�
				{
					uf.BeginFunds = atof(word); // ģ�������ʹ�ÿ�ʼ�ʽ�
					printf("���ʲ���%f\n", atof(word));
				}
			}

			if (nextchar == '\n')
			{
				// ����
				rows++;
				cols = 0;

			}
			else if (nextchar != '\t' && nextchar != 0)
			{
				word[wordlen] = nextchar;
				wordlen++;		
			}
		}

		printf("�����ʽ�%f �����ʽ�%f ��ʼ�ʽ�%f\n", uf.TotalFunds, uf.LockFunds, uf.BeginFunds);
		// ��䷢�ͻ�����
		sendlen = sizeof(VC_UserFunds);
		memcpy(sendbuffer, &uf, sendlen);
		sendcommand = VK_USER_FUNDS;
	}
	else
	{
		strcpy((char*)sendbuffer, "��ѯʧ��");
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 1;
	}
}


void handle_zijingufen(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ���ѯ�ɷ�ָ��\n", dwThreadId);

	int ret = tdxquerydata(1, Result, ErrInfo);

	if (ret == 1)
	{
		// ���عɷ���Ϣ
		struct VC_ZIJINGUFEN zjgf[100];
		memset(zjgf, 0, sizeof(VC_ZIJINGUFEN) * 100);

		int rows = 1;
		int cols = 0;
		int wordlen = 0;
		char word[100];
		for (unsigned long i = 0; i <= strlen(Result); i++)
		{
			// ���һ���ַ�
			char nextchar = Result[i];

			if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
			{
				// ǰһ���ַ�������
				cols++;
				word[wordlen] = 0;

				wordlen = 0;

				if (rows > 1)
				{
					if (cols == 1) // ֤ȯ����
					{
						strcpy(zjgf[rows-2].stockcode, word);
					}
					if (cols == 2) // ֤ȯ����
					{
						strcpy(zjgf[rows-2].stockname, word);
					}
					if (cols == 3) // �ο��ֹ�
					{
						zjgf[rows-2].num = atof(word); // ֤ȯ����
					}
					if (cols == 4) // ���óֹ�
					{
						zjgf[rows-2].CansellNum = atof(word); // ��������
					}
					if (cols == 5) // �ɱ���
					{
						zjgf[rows-2].buyPrice = atof(word);
					}
					if (cols == 6) // ��ǰ��
					{
						zjgf[rows-2].NewPrice = atof(word);
					}
					if (cols == 8) // ������ֵ
					{
						zjgf[rows-2].NewShizhi = atof(word);
					}
					if (cols == 9) // ����ӯ��
					{
						zjgf[rows-2].FloatYk = atof(word);
					}
					if (cols == 10) // ӯ������
					{
						zjgf[rows-2].Ykbl = atof(word);
					}
				}						
			}
			if (nextchar == '\n')
			{
				// ����
				rows++;
				cols = 0;

			}
			else if (nextchar != '\t' && nextchar != 0)
			{
				word[wordlen] = nextchar;
				wordlen++;		
			}
		}

		// ��䷢�ͻ�����
		if(rows > 1)
		{
			sendlen = sizeof(VC_ZIJINGUFEN) * (rows-1);
			memcpy(sendbuffer, zjgf, sendlen);
		}
		else
		{
			strcpy((char*)sendbuffer, "�޼�¼");
			sendlen = strlen((char*)sendbuffer) + 1;
		}
		sendcommand = VK_ZIJINGUFEN;				
	}
	else
	{
		strcpy((char*)sendbuffer, "��ѯʧ��");
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 1;
	}
}

void handle_weituo(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ���ѯ����ί��ָ��\n", dwThreadId);

	int ret = tdxquerydata(2, Result, ErrInfo);

	if (ret == 1)
	{
		// ���ص���ί����Ϣ
		struct VC_WEITUO weituo[100];
		memset(weituo, 0, sizeof(VC_WEITUO) * 100);

		int rows = 1;
		int cols = 0;
		int wordlen = 0;
		char word[100];
		for (unsigned long i = 0; i <= strlen(Result); i++)
		{
			// ���һ���ַ�
			char nextchar = Result[i];

			if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
			{
				// ǰһ���ַ�������
				cols++;
				word[wordlen] = 0;

				wordlen = 0;

				if (rows > 1)
				{
					if (cols == 3)	// ֤ȯ����
					{
						strcpy(weituo[rows-2].stockcode, word);
					}
					if (cols == 4)	// ֤ȯ����
					{
						strcpy(weituo[rows-2].stockname, word);
					}
					if (cols == 20)	// ί�б��
					{
						weituo[rows-2].id = atoi(word);
					}
					if (cols == 1)	// ί��ʱ��
					{
						// 18:43:10 -> 184310
						char time[7];
						memcpy(time, word, 2);
						memcpy(&time[2], &word[3], 2);
						memcpy(&time[4], &word[6], 2);
						time[6] = '\0';
						weituo[rows-2].creaTime = atoi(time);
					}
					if (cols == 5) // ������־: 0��1��
					{
						// ������־��0����, 1����, 2������, 3������, 4����ת��, 5����תȡ, 6�ɱ��ֺ�, 7ת���ɱ�
						weituo[rows-2].trdeType = atoi(word);
					}
					if (cols == 8)	// ί��״̬
					{
						// ����״̬��0δ�ɽ�, 1�ѳ�, 2�ѳ���, 3�ѳ���
						if (strcmp(word, "�ѳ�") == 0)
						{
							if (weituo[rows-2].trdeType == 0) // 0�����־
							{
								weituo[rows-2].trdeStatus = 2; 
							}
							else if (weituo[rows-2].trdeType == 1) // 1������־
							{
								weituo[rows-2].trdeStatus = 3;
							}
						}
						else if (strcmp(word, "�ѳ�") == 0)
						{
							weituo[rows-2].trdeStatus = 1;
						}
						else
						{
							weituo[rows-2].trdeStatus = 0;
						}
					}
					if (cols == 9) // ί�м۸�
					{
						weituo[rows-2].price = atof(word);
					}
					if (cols == 10) // ί������
					{
						weituo[rows-2].num = atof(word);
					}
					if (cols == 11) // �ɽ��۸�
					{
						weituo[rows-2].turnoverPrice = atof(word);
					}
					if (cols == 12) // �ɽ�����
					{
						weituo[rows-2].turnoverNum = atof(word);
					}
					if (cols == 13) // û�б��۷�ʽ����Ϣ
					{
						// ���۷�ʽ��0�޼ۣ�1�м�
						weituo[rows-2].reaPortType = 1; // Ĭ��Ϊ�м�
					}
				}						
			}
			if (nextchar == '\n')
			{
				// ����
				rows++;
				cols = 0;

			}
			else if (nextchar != '\t' && nextchar != 0)
			{
				word[wordlen] = nextchar;
				wordlen++;		
			}
		}

		// ��䷢�ͻ�����
		if(rows > 1)
		{
			sendlen = sizeof(VC_WEITUO) * (rows-1);
			memcpy(sendbuffer, weituo, sendlen);
		}
		else
		{
			strcpy((char*)sendbuffer, "�޼�¼");
			sendlen = strlen((char*)sendbuffer) + 1;
		}

		sendcommand = VK_WEITUO;
	}
	else
	{
		strcpy((char*)sendbuffer, "��ѯʧ��");
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 1;
	}
}
void handle_chengjiao(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ���ѯ����ί��ָ��\n", dwThreadId);

	int ret = tdxquerydata(3, Result, ErrInfo);

	if (ret == 1)
	{

		// ���ص��ճɽ���Ϣ
		struct VC_CHENGJIAO chengjiao[100];
		memset(chengjiao, 0, sizeof(VC_CHENGJIAO) * 100);

		int rows = 1;
		int cols = 0;
		int wordlen = 0;
		char word[100];
		for (unsigned long i = 0; i <= strlen(Result); i++)
		{
			// ���һ���ַ�
			char nextchar = Result[i];

			if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
			{
				// ǰһ���ַ�������
				cols++;
				word[wordlen] = 0;

				wordlen = 0;

				if (rows > 1)
				{
					if (cols == 3)	// ֤ȯ����
					{
						strcpy(chengjiao[rows-2].stockcode, word);
					}
					if (cols == 4)	// ֤ȯ����
					{
						strcpy(chengjiao[rows-2].stockname, word);
					}
					if (cols == 1)	// �ɽ�ʱ��
					{
						// 18:43:10 -> 184310
						char time[7];
						memcpy(time, word, 2);
						memcpy(&time[2], &word[3], 2);
						memcpy(&time[4], &word[6], 2);
						time[6] = '\0';
						chengjiao[rows-2].creatTime = atoi(time);
					}
					if (cols == 5) // ������־: 0��1��
					{
						// ������־��0����, 1����, 2������, 3������, 4����ת��, 5����תȡ, 6�ɱ��ֺ�, 7ת���ɱ�
						chengjiao[rows-2].trdeType = atoi(word);
					}
					if (cols == 12)	// �ɽ�״̬
					{
						// ����״̬��0δ�ɽ�, 1�ѳ�, 2�ѳ���, 3�ѳ���
						if (strcmp(word, "�ѳ�") == 0)
						{
							if (chengjiao[rows-2].trdeType == 0) // 0�����־
							{
								chengjiao[rows-2].trdeStatus = 2; 
							}
							else if (chengjiao[rows-2].trdeType == 1) // 1������־
							{
								chengjiao[rows-2].trdeStatus = 3;
							}
						}
						else if (strcmp(word, "�ѳ�") == 0)
						{
							chengjiao[rows-2].trdeStatus = 1;
						}
						else
						{
							chengjiao[rows-2].trdeStatus = 0;
						}
					}
					if (cols == 16)	// ί�б��
					{
						chengjiao[rows-2].id = atoi(word);
					}
					if (cols == 8) // �ɽ��۸�
					{
						chengjiao[rows-2].turnoverPrice = atof(word);
					}
					if (cols == 9) // �ɽ�����
					{
						chengjiao[rows-2].turnoverNum = atof(word);
					}
					if (cols == 10) // �ɽ����
					{
						chengjiao[rows-2].turnoverFunds = atof(word);
					}
				}						
			}
			if (nextchar == '\n')
			{
				// ����
				rows++;
				cols = 0;

			}
			else if (nextchar != '\t' && nextchar != 0)
			{
				word[wordlen] = nextchar;
				wordlen++;		
			}
		}

		// ��䷢�ͻ�����
		if(rows > 1)
		{
			sendlen = sizeof(VC_CHENGJIAO) * (rows-1);
			memcpy(sendbuffer, chengjiao, sendlen);
		}
		else
		{
			strcpy((char*)sendbuffer, "�޼�¼");
			sendlen = strlen((char*)sendbuffer) + 1;
		}

		sendcommand = VK_CHENGJIAO;
	}
	else
	{
		strcpy((char*)sendbuffer, "��ѯʧ��");
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 1;
	}
}
void handle_chedan(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ���ѯ����ί��ָ��\n", dwThreadId);

	int ret = tdxquerydata(4, Result, ErrInfo);

	if (ret == 1)
	{

		// ���س�����Ϣ
		struct VC_CHE_DAN chedan[100];
		memset(chedan, 0, sizeof(VC_CHE_DAN) * 100);

		int rows = 1;
		int cols = 0;
		int wordlen = 0;
		char word[100];
		for (unsigned long i = 0; i <= strlen(Result); i++)
		{
			// ���һ���ַ�
			char nextchar = Result[i];

			if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
			{
				// ǰһ���ַ�������
				cols++;
				word[wordlen] = 0;

				wordlen = 0;

				if (rows > 1)
				{
					if (cols == 3)	// ֤ȯ����
					{
						strcpy(chedan[rows-2].stockcode, word);
					}
					if (cols == 4)	// ֤ȯ����
					{
						strcpy(chedan[rows-2].stockname, word);
					}
					if (cols == 1)	// ί��ʱ��
					{
						// 18:43:10 -> 184310
						char time[7];
						memcpy(time, word, 2);
						memcpy(&time[2], &word[3], 2);
						memcpy(&time[4], &word[6], 2);
						time[6] = '\0';
						chedan[rows-2].time = atoi(time);
					}
					if (cols == 5) // ������־: 0��1��
					{
						// ������־��0����, 1����, 2������, 3������, 4����ת��, 5����תȡ, 6�ɱ��ֺ�, 7ת���ɱ�
						chedan[rows-2].tradetag = atoi(word);
					}
					if (cols == 8)	// ί��״̬
					{
						// ����״̬��0δ�ɽ�, 1�ѳ�, 2�ѳ���, 3�ѳ���
						if (strcmp(word, "�ѳ�") == 0)
						{
							if (chedan[rows-2].tradetag == 0) // 0�����־
							{
								chedan[rows-2].status = 2; 
							}
							else if (chedan[rows-2].tradetag == 1) // 1������־
							{
								chedan[rows-2].status = 3;
							}
						}
						else if (strcmp(word, "�ѳ�") == 0)
						{
							chedan[rows-2].status = 1;
						}
						else
						{
							chedan[rows-2].status = 0;
						}
					}
					if (cols == 9) // ί�м۸�
					{
						chedan[rows-2].trdeprice = atof(word);
					}
					if (cols == 10) // ί������
					{
						chedan[rows-2].trdenum = atof(word);
					}
					if (cols == 2)	// ί�б��
					{
						chedan[rows-2].id = chedan[rows-2].tradeid = atoi(word);
						strcpy(chedan[rows-2].hashCode, word);
					}
					if (cols == 11) // �ɽ��۸�
					{
						chedan[rows-2].cjprice = atof(word);
					}
					if (cols == 12) // �ɽ�����
					{
						chedan[rows-2].cjnum = atof(word);
					}
					if (cols == 13) // û�б��۷�ʽ����Ϣ
					{
						// ���۷�ʽ��0�޼ۣ�1�м�
						chedan[rows-2].reporttype = 1; // Ĭ��Ϊ�м�
					}
				}						
			}
			if (nextchar == '\n')
			{
				// ����
				rows++;
				cols = 0;

			}
			else if (nextchar != '\t' && nextchar != 0)
			{
				word[wordlen] = nextchar;
				wordlen++;		
			}
		}

		// ��䷢�ͻ�����
		if(rows > 1)
		{
			sendlen = sizeof(VC_CHE_DAN) * (rows-1);
			memcpy(sendbuffer, chedan, sendlen);
		}
		else
		{
			strcpy((char*)sendbuffer, "�޼�¼");
			sendlen = strlen((char*)sendbuffer) + 1;
		}

		sendcommand = VK_CHE_DAN;
	}
	else
	{
		strcpy((char*)sendbuffer, "��ѯʧ��");
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 1;
	}
}
void handle_chaxun5d(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ���ѯ��Ʊ�嵵����ָ��\n", dwThreadId);

	if(delen == sizeof(SockData))
	{
		// ��ȡ��Ʊ����
		struct SockData *sd = (struct SockData *)debuffer;
		printf("���߳�%d: ����ѯ�Ĺ�Ʊ����Ϊ��%s\n", dwThreadId, sd->stockCode);

		int ret = tdxgetquote(sd->stockCode, Result, ErrInfo);
		if (ret == 1)
		{
			// ���ع�Ʊ������Ϣ
			struct SockData return_sd;
			memset(&return_sd.Stock_data, 0, sizeof(tagRCV_REPORT_STRUCTEx));
			sd = &return_sd;

			int rows = 1;
			int cols = 0;
			int wordlen = 0;
			char word[100];
			for (unsigned long i = 0; i <= strlen(Result); i++)
			{
				// ���һ���ַ�
				char nextchar = Result[i];

				if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
				{
					// ǰһ���ַ�������
					cols++;
					word[wordlen] = 0;

					wordlen = 0;

					if (rows > 1)
					{
						if (cols == 1)	// ֤ȯ����
						{
							strcpy(sd->Stock_data.m_szLabel, word);
						}
						if (cols == 2)	// ֤ȯ����
						{
							strcpy(sd->Stock_data.m_szName, word);
						}
						if (cols == 3)	// ����
						{
							sd->Stock_data.m_fLastClose = atof(word);
						}
						if (cols == 4) // ��
						{
							sd->Stock_data.m_fOpen = atof(word);
						}
						if (cols == 6)	// ��ǰ��
						{
							sd->Stock_data.m_fNewPrice = atof(word);
						}
						if (cols == 7)	// ��һ��
						{
							sd->Stock_data.m_fBuyPrice[0] = atof(word);
						}
						if (cols == 8)	// �����
						{
							sd->Stock_data.m_fBuyPrice[1] = atof(word);
						}
						if (cols == 9)	// ������
						{
							sd->Stock_data.m_fBuyPrice[2] = atof(word);
						}
						if (cols == 10)	// ���ļ�
						{
							sd->Stock_data.m_fBuyPrice4 = atof(word);
						}
						if (cols == 11)	// �����
						{
							sd->Stock_data.m_fBuyPrice5 = atof(word);
						}
						if (cols == 12)	// ��һ��
						{
							sd->Stock_data.m_fBuyVolume[0] = atof(word);
						}
						if (cols == 13)	// �����
						{
							sd->Stock_data.m_fBuyVolume[1] = atof(word);
						}
						if (cols == 14)	// ������
						{
							sd->Stock_data.m_fBuyVolume[2] = atof(word);
						}
						if (cols == 15)	// ������
						{
							sd->Stock_data.m_fBuyVolume4 = atof(word);
						}
						if (cols == 16)	// ������
						{
							sd->Stock_data.m_fBuyVolume5 = atof(word);
						}
						if (cols == 17)	// ��һ��
						{
							sd->Stock_data.m_fSellPrice[0] = atof(word);
						}
						if (cols == 18)	// ������
						{
							sd->Stock_data.m_fSellPrice[1] = atof(word);
						}
						if (cols == 19)	// ������
						{
							sd->Stock_data.m_fSellPrice[2] = atof(word);
						}
						if (cols == 20)	// ���ļ�
						{
							sd->Stock_data.m_fSellPrice4 = atof(word);
						}
						if (cols == 21)	// �����
						{
							sd->Stock_data.m_fSellPrice5 = atof(word);
						}
						if (cols == 22)	// ��һ��
						{
							sd->Stock_data.m_fSellVolume[0] = atof(word);
						}
						if (cols == 23)	// ������
						{
							sd->Stock_data.m_fSellVolume[1] = atof(word);
						}
						if (cols == 24)	// ������
						{
							sd->Stock_data.m_fSellVolume[2] = atof(word);
						}
						if (cols == 25)	// ������
						{
							sd->Stock_data.m_fSellVolume4 = atof(word);
						}
						if (cols == 26)	// ������
						{
							sd->Stock_data.m_fSellVolume5 = atof(word);
						}
					}						
				}
				if (nextchar == '\n')
				{
					// ����
					rows++;
					cols = 0;

				}
				else if (nextchar != '\t' && nextchar != 0)
				{
					word[wordlen] = nextchar;
					wordlen++;		
				}
			}

			// ��䷢�ͻ�����
			if(rows > 1)
			{
				sendlen = sizeof(SockData);
				memcpy(sendbuffer, &return_sd, sendlen);
			}
			else
			{
				strcpy((char*)sendbuffer, "�޼�¼");
				sendlen = strlen((char*)sendbuffer) + 1;
			}

			sendcommand = STOCK_DATA;
		}
		else
		{
			strcpy((char*)sendbuffer, "��ѯʧ��");
			sendlen = strlen((char*)sendbuffer) + 1;
			sendcommand = 1;
		}


	}
}
void handle_weituolishi(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ�ί����ʷ��ѯָ��\n", dwThreadId);

	if (delen == sizeof(Android_DateTimes))
	{
		// ��ȡ��ѯ��ʼ���ںͽ�������
		struct Android_DateTimes *ad = (struct Android_DateTimes *)debuffer;

		char start_date[10];
		char end_date[10];
		sprintf(start_date, "%04d%02d%02d", ad->StartYear, ad->StartMonth, ad->StartDay);
		sprintf(end_date, "%04d%02d%02d", ad->EndYear, ad->EndMonth, ad->EndDay);
		printf("���߳�%d: ��ѯ���ڣ�%s -> %s\n", dwThreadId, start_date, end_date);

		int ret = tdxqueryhistory(0, start_date, end_date, Result, ErrInfo);

		if (ret == 1)
		{
			struct VC_WEITUO_LISHI weituo[1000];
			memset(weituo, 0, sizeof(VC_WEITUO_LISHI) * 1000);

			int rows = 1;
			int cols = 0;
			int wordlen = 0;
			char word[100];
			for (unsigned long i = 0; i <= strlen(Result); i++)
			{
				// ���һ���ַ�
				char nextchar = Result[i];

				if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
				{
					// ǰһ���ַ�������
					cols++;
					word[wordlen] = 0;

					wordlen = 0;

					if (rows > 1)
					{
						if (cols == 4)	// ֤ȯ����
						{
							strcpy(weituo[rows-2].stockcode, word);
						}
						if (cols == 5)	// ֤ȯ����
						{
							strcpy(weituo[rows-2].stockname, word);
						}
						if (cols == 1)	// ί������
						{
							weituo[rows-2].Date = atoi(word);
						}
						if (cols == 2)	// ί��ʱ��
						{
							// 18:43:10 -> 184310
							char time[7];
							memcpy(time, word, 2);
							memcpy(&time[2], &word[3], 2);
							memcpy(&time[4], &word[6], 2);
							time[6] = '\0';
							weituo[rows-2].Time = atoi(time);
						}
						if (cols == 6) // ������־: 0��1��
						{
							// ������־��0����, 1����, 2������, 3������, 4����ת��, 5����תȡ, 6�ɱ��ֺ�, 7ת���ɱ�
							weituo[rows-2].trdeType = atoi(word);
						}
						if (cols == 9)	// ί��״̬
						{
							// ����״̬��0δ�ɽ�, 1�ѳ�, 2�ѳ���, 3�ѳ���
							if (strcmp(word, "�ѳ�") == 0)
							{
								if (weituo[rows-2].trdeType == 0) // 0�����־
								{
									weituo[rows-2].trdeStatus = 2; 
								}
								else if (weituo[rows-2].trdeType == 1) // 1������־
								{
									weituo[rows-2].trdeStatus = 3;
								}
							}
							else if (strcmp(word, "�ѳ�") == 0)
							{
								weituo[rows-2].trdeStatus = 1;
							}
							else
							{
								weituo[rows-2].trdeStatus = 0;
							}
						}
						if (cols == 10) // ί�м۸�
						{
							weituo[rows-2].Eprice = atof(word);
						}
						if (cols == 11) // ί������
						{
							weituo[rows-2].Enum = atof(word);
						}
						if (cols == 12) // �ɽ��۸�
						{
							weituo[rows-2].Tprice = atof(word);
						}
						if (cols == 13) // �ɽ�����
						{
							weituo[rows-2].Tnum = atof(word);
						}
						if (cols == 19) // ί�б��
						{
							weituo[rows-2].Eno = atoi(word);
						}
						if (cols == 14) // û�б��۷�ʽ����Ϣ
						{
							// ���۷�ʽ��0�޼ۣ�1�м�
							weituo[rows-2].reaPortType = 1; // Ĭ��Ϊ�м�
						}
					}						
				}
				if (nextchar == '\n')
				{
					// ����
					rows++;
					cols = 0;

				}
				else if (nextchar != '\t' && nextchar != 0)
				{
					word[wordlen] = nextchar;
					wordlen++;		
				}
			}

			// ��䷢�ͻ�����
			if(rows > 1)
			{
				sendlen = sizeof(VC_WEITUO_LISHI) * (rows-1);
				memcpy(sendbuffer, weituo, sendlen);
			}
			else
			{
				strcpy((char*)sendbuffer, "�޼�¼");
				sendlen = strlen((char*)sendbuffer) + 1;
			}

			sendcommand = VK_WEITUO_LISHI;
		}
		else
		{
			strcpy((char*)sendbuffer, "��ѯʧ��");
			sendlen = strlen((char*)sendbuffer) + 1;
			sendcommand = 1;
		}

	}
}
void handle_chengjiaolishi(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ��ɽ���ʷ��ѯָ��\n", dwThreadId);

	if (delen == sizeof(Android_DateTimes))
	{
		// ��ȡ��ѯ��ʼ���ںͽ�������
		struct Android_DateTimes *ad = (struct Android_DateTimes *)debuffer;

		char start_date[10];
		char end_date[10];
		sprintf(start_date, "%04d%02d%02d", ad->StartYear, ad->StartMonth, ad->StartDay);
		sprintf(end_date, "%04d%02d%02d", ad->EndYear, ad->EndMonth, ad->EndDay);
		printf("���߳�%d: ��ѯ���ڣ�%s -> %s\n", dwThreadId, start_date, end_date);

		int ret = tdxqueryhistory(1, start_date, end_date, Result, ErrInfo);

		if (ret == 1)
		{

			// ���ص��ճɽ���Ϣ
			struct VC_CHENGJIO_LISHI chengjiao[1000];
			memset(chengjiao, 0, sizeof(VC_CHENGJIO_LISHI) * 1000);

			int rows = 1;
			int cols = 0;
			int wordlen = 0;
			char word[100];
			for (unsigned long i = 0; i <= strlen(Result); i++)
			{
				// ���һ���ַ�
				char nextchar = Result[i];

				if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
				{
					// ǰһ���ַ�������
					cols++;
					word[wordlen] = 0;

					wordlen = 0;

					if (rows > 1)
					{
						if (cols == 4)	// ֤ȯ����
						{
							strcpy(chengjiao[rows-2].stockcode, word);
						}
						if (cols == 5)	// ֤ȯ����
						{
							strcpy(chengjiao[rows-2].stockname, word);
						}
						if (cols == 1)	// �ɽ�����
						{									
							chengjiao[rows-2].Date = atoi(word);
						}
						if (cols == 2)	// �ɽ�ʱ��
						{
							// 18:43:10 -> 184310
							char time[7];
							memcpy(time, word, 2);
							memcpy(&time[2], &word[3], 2);
							memcpy(&time[4], &word[6], 2);
							time[6] = '\0';
							chengjiao[rows-2].Time = atoi(time);
						}
						if (cols == 6) // ������־: 0��1��
						{
							// ������־��0����, 1����, 2������, 3������, 4����ת��, 5����תȡ, 6�ɱ��ֺ�, 7ת���ɱ�
							chengjiao[rows-2].trdeType = atoi(word);
						}
						if (cols == 8) // �ɽ��۸�
						{
							chengjiao[rows-2].Tprice = atof(word);
						}
						if (cols == 9) // �ɽ�����
						{
							chengjiao[rows-2].Tnum = atof(word);
						}
						if (cols == 12) // ί�б��
						{
							chengjiao[rows-2].Tno = atoi(word);
						}
					}						
				}
				if (nextchar == '\n')
				{
					// ����
					rows++;
					cols = 0;

				}
				else if (nextchar != '\t' && nextchar != 0)
				{
					word[wordlen] = nextchar;
					wordlen++;		
				}
			}

			// ��䷢�ͻ�����
			if(rows > 1)
			{
				sendlen = sizeof(VC_CHENGJIO_LISHI) * (rows-1);
				memcpy(sendbuffer, chengjiao, sendlen);
			}
			else
			{
				strcpy((char*)sendbuffer, "�޼�¼");
				sendlen = strlen((char*)sendbuffer) + 1;
			}

			sendcommand = VK_CHENGJIO_LISHI;
		}
		else
		{
			strcpy((char*)sendbuffer, "��ѯʧ��");
			sendlen = strlen((char*)sendbuffer) + 1;
			sendcommand = 1;
		}
	}
}
void handle_zijinliushui(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ��ʽ���ʷ��ѯָ��\n", dwThreadId);

	if (delen == sizeof(Android_DateTimes))
	{
		// ��ȡ��ѯ��ʼ���ںͽ�������
		struct Android_DateTimes *ad = (struct Android_DateTimes *)debuffer;

		char start_date[10];
		char end_date[10];
		sprintf(start_date, "%04d%02d%02d", ad->StartYear, ad->StartMonth, ad->StartDay);
		sprintf(end_date, "%04d%02d%02d", ad->EndYear, ad->EndMonth, ad->EndDay);
		printf("���߳�%d: ��ѯ���ڣ�%s -> %s\n", dwThreadId, start_date, end_date);

		// ��ȡ�ʽ���ʷ�б�
		int ret = tdxqueryhistory(3, start_date, end_date, Result, ErrInfo);

		if (ret == 1)
		{

			// ���ص��ճɽ���Ϣ
			struct VC_ZIJINLIUSHUI zjls[1000];
			memset(zjls, 0, sizeof(VC_ZIJINLIUSHUI) * 1000);

			int rows = 1;
			int cols = 0;
			int wordlen = 0;
			char word[100];
			for (unsigned long i = 0; i <= strlen(Result); i++)
			{
				// ���һ���ַ�
				char nextchar = Result[i];

				if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
				{
					// ǰһ���ַ�������
					cols++;
					word[wordlen] = 0;

					wordlen = 0;

					if (rows > 1)
					{
						if (cols == 4)	// ֤ȯ����
						{
							strcpy(zjls[rows-2].stockcode, word);
						}
						if (cols == 5)	// ֤ȯ����
						{
							strcpy(zjls[rows-2].stockname, word);
						}
						if (cols == 1)	// �ɽ�����
						{									
							zjls[rows-2].Date = atoi(word);
						}
						if (cols == 2)	// �ɽ�ʱ��
						{
							// 18:43:10 -> 184310
							char time[7];
							memcpy(time, word, 2);
							memcpy(&time[2], &word[3], 2);
							memcpy(&time[4], &word[6], 2);
							time[6] = '\0';
							zjls[rows-2].Time = atoi(time);
						}
						if (cols == 6) // �ɽ��۸�
						{
							zjls[rows-2].price = atof(word);
						}
						if (cols == 7) // �ɽ�����
						{
							zjls[rows-2].num = atof(word);
						}
						if (cols == 16) // �������
						{
							zjls[rows-2].funds = atof(word);
						}
						if (cols == 17) // �ʽ𱾴����
						{
							zjls[rows-2].canusefunds = atof(word);
						}
					}						
				}
				if (nextchar == '\n')
				{
					// ����
					rows++;
					cols = 0;

				}
				else if (nextchar != '\t' && nextchar != 0)
				{
					word[wordlen] = nextchar;
					wordlen++;		
				}
			}

			// ��䷢�ͻ�����
			if(rows > 1)
			{
				sendlen = sizeof(VC_ZIJINLIUSHUI) * (rows-1);
				memcpy(sendbuffer, zjls, sendlen);
			}
			else
			{
				strcpy((char*)sendbuffer, "�޼�¼");
				sendlen = strlen((char*)sendbuffer) + 1;
			}

			sendcommand = VK_ZIJINLIUSHUI;
		}
		else
		{
			strcpy((char*)sendbuffer, "��ѯʧ��");
			sendlen = strlen((char*)sendbuffer) + 1;
			sendcommand = 1;
		}
	}
}
void handle_jiaogedan(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("���߳�%d: ���յ������ʷ��ѯָ��\n", dwThreadId);

	if (delen == sizeof(Android_DateTimes))
	{
		// ��ȡ��ѯ��ʼ���ںͽ�������
		struct Android_DateTimes *ad = (struct Android_DateTimes *)debuffer;

		char start_date[10];
		char end_date[10];
		sprintf(start_date, "%04d%02d%02d", ad->StartYear, ad->StartMonth, ad->StartDay);
		sprintf(end_date, "%04d%02d%02d", ad->EndYear, ad->EndMonth, ad->EndDay);
		printf("���߳�%d: ��ѯ���ڣ�%s -> %s\n", dwThreadId, start_date, end_date);

		// ��ȡ�����ʷ�б�

		int ret = tdxqueryhistory(2, start_date, end_date, Result, ErrInfo);

		if (ret == 1)
		{

			// ���ص��ճɽ���Ϣ
			struct VC_JIAOGEDAN jgd[1000];
			memset(jgd, 0, sizeof(VC_JIAOGEDAN) * 1000);

			int rows = 1;
			int cols = 0;
			int wordlen = 0;
			char word[100];
			for (unsigned long i = 0; i <= strlen(Result); i++)
			{
				// ���һ���ַ�
				char nextchar = Result[i];

				if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
				{
					// ǰһ���ַ�������
					cols++;
					word[wordlen] = 0;

					wordlen = 0;

					if (rows > 1)
					{
						if (cols == 2)	// ֤ȯ����
						{
							strcpy(jgd[rows-2].stockcode, word);
						}
						if (cols == 3)	// ֤ȯ����
						{
							strcpy(jgd[rows-2].stockname, word);
						}
						if (cols == 1)	// �ɽ�����
						{									
							jgd[rows-2].Date = atoi(word);
						}
						if (cols == 16)	// �ɽ�ʱ��
						{
							// 18:43:10 -> 184310
							char time[7];
							memcpy(time, word, 2);
							memcpy(&time[2], &word[3], 2);
							memcpy(&time[4], &word[6], 2);
							time[6] = '\0';
							jgd[rows-2].Time = atoi(time);
						}
						if (cols == 5) // �ɽ��۸�
						{
							jgd[rows-2].Tprice = atof(word);
						}
						if (cols == 6) // �ɽ�����
						{
							jgd[rows-2].Tnum = atof(word);
						}
						if (cols == 7) // �ɽ����
						{
							jgd[rows-2].TFunds = atof(word);
						}
						if (cols == 7) // ������
						{
							jgd[rows-2].Funds = atof(word); // �������
						}
						if (cols == 8) // ������
						{
							jgd[rows-2].Sxfee = atof(word); // Ӷ��
						}
						if (cols == 10) // ������
						{
							jgd[rows-2].Ghfee = atof(word); // ������
						}
						if (cols == 9) // ӡ��˰
						{
							jgd[rows-2].Yhfee = atof(word); // ӡ��˰
						}
						if (cols == 11) // ���ӷ�
						{
							jgd[rows-2].Qtfee = atof(word); // ������
						}
						if (cols == 15) // ί�б��
						{
							jgd[rows-2].Tno = atoi(word);
						}
					}						
				}
				if (nextchar == '\n')
				{
					// ����
					rows++;
					cols = 0;

				}
				else if (nextchar != '\t' && nextchar != 0)
				{
					word[wordlen] = nextchar;
					wordlen++;		
				}
			}

			// ��䷢�ͻ�����
			if(rows > 1)
			{
				sendlen = sizeof(VC_JIAOGEDAN) * (rows-1);
				memcpy(sendbuffer, jgd, sendlen);
			}
			else
			{
				strcpy((char*)sendbuffer, "�޼�¼");
				sendlen = strlen((char*)sendbuffer) + 1;
			}

			sendcommand = VK_JIAOGEDAN;
		}
		else
		{
			strcpy((char*)sendbuffer, "��ѯʧ��");
			sendlen = strlen((char*)sendbuffer) + 1;
			sendcommand = 1;
		}
	}
}

void handle_chedanorder(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo,UserList current_user)
{
	printf("���߳�%d: ���յ�����ָ��\n", dwThreadId);
	debuffer[delen] = 0;
	printf("���߳�%d: TradeID = %s\n", dwThreadId, debuffer);

	// �������
	int ret = tdxcancelorder((char*)debuffer, 1, Result, ErrInfo);
	//int ret=1;
	if (ret == 1)
	{
		sendlen = strlen(Result) + 1;
		memcpy(sendbuffer, Result, sendlen);
		sendcommand = 1;
		//�����û�������Ϣ
		char current_t[20],current_d[20];
		user_current_time(current_d,current_t);
		char TradeInfo[512];
		//sprintf(TradeInfo,"%s%s %s %s %s %d","\n",current_d,current_t,"klklklih0i99","��",0);
		sprintf(TradeInfo,"%s%s %s %s %s %d","\n",current_d,current_t,debuffer,"��",0);
		user_Record(current_user->userInfo,TradeInfo);

	}
	else
	{
		strcpy((char*)sendbuffer, "����ʧ��");
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 0;
	}
}
int handle_msg(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId,UserList *current_user)
{

	char *Result = new char[10*1024*1024];
	char *ErrInfo = new char[1024*1024];

	// ������Ϣ
	printf("���߳�%d: ��ʼ������Ϣ...\n\n", dwThreadId);
	if (command == VK_LOGIN)//��¼ʱ��Ҫ��֤��Ϣ
	{
		handle_login(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo,current_user);
	}
	else if (command == BUY)//�µ��ɹ�ʱ��Ҫ��¼��Ϣ
	{
		handle_buy(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo,*current_user);
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
		handle_chedanorder(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo,*current_user);
	}
	else
	{
		strcpy((char*)sendbuffer, "��ã�TCP�ͻ��ˣ����Ƿ����!\n");
		printf("���߳�%d: ������������%s\n", dwThreadId, sendbuffer);
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 0;
	}

	delete []Result;
	delete []ErrInfo;

	return 1;
}