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

void handle_login(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo,UserList *current_user)
{
	printf("子线程%d: 处理登录指令\n", dwThreadId);
	if(delen == sizeof(login))
	{
		login *lg = (login*)debuffer;
		printf("子线程%d: 用户名：%s, 密码：%s\n", dwThreadId, lg->UserId, lg->UserPass);
		//验证用户信息
		*current_user=user_Find_User(User,*lg);
		printf("当前登录用户：%s \n",(**current_user).userInfo.UserId);
		// 返回用户信息
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

		// 查询用户资金信息
		int ret = tdxquerydata(0, Result, ErrInfo);

		if (ret == 1)
		{
			int width = 8;
			int rows = 1;
			int cols = 0;
			int wordlen = 0;
			char word[100];
			printf("第1行：");
			for (unsigned long i = 0; i <= strlen(Result); i++)
			{
				// 获得一个字符
				char nextchar = Result[i];

				if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
				{
					// 前一个字符串结束
					cols++;
					word[wordlen] = 0;

					wordlen = 0;

					if (rows == 2 && cols == 3) // 可用资金
					{							
						info.FundsTotal = atof(word);
					}
					if (rows == 2 && cols == 4) // 冻结资金
					{
						info.LockFunds = atof(word);
					}
					if (rows == 2 && cols == 6) // 总资产
					{
						info.BeginFunds = atof(word); // 模拟服务器使用开始资金
					}
				}

				if (nextchar == '\n')
				{
					// 换行
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


		// 填充发送缓冲区
		sendlen = sizeof(UserInfo);
		memcpy(sendbuffer, (char*)&info, sendlen);
		sendcommand = VK_LOGIN;
		printf("填充发送缓冲区，大小%d\n", sendlen);
	}
}

void handle_buy(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo,UserList current_user)
{
	printf("子线程%d: 处理下单指令\n", dwThreadId);
	if(delen == sizeof(TradeForm))
	{
		struct TradeForm *tf = (struct TradeForm*)debuffer;
		printf("子线程%d: TradeType %s, TradeMode %s, Price %.2f\n", dwThreadId, tradetype[tf->TradeType], reaPortType[tf->reaPortType], tf->Price);

		// 开始下单
		int ret = tdxsendorder(tf->TradeType, tf->reaPortType, "", tf->SockCode, tf->Price, tf->Num, Result, ErrInfo);

		//测试
		//int ret=1;
		if (ret == 1)//下单成功
		{
			char tmp1[10], tmp2[10];
			char tradeid[10], mark[10];
			sscanf(Result, "%s%s%s%s", tmp1, tmp2, tradeid, mark);

			char return_str[20];
			sprintf(return_str, "%s\t%s", tradeid, mark);

			//记录用户下单信息
			char current_t[20],current_d[20];//tradeid下单编号
			user_current_time(current_d,current_t);
			char TradeInfo[512];
			//sprintf(TradeInfo,"%s%s %s %s %s %d","\n",current_d,current_t,"909090",tradetype[tf->TradeType],tf->Num);
			sprintf(TradeInfo,"%s%s %s %s %s %d","\n",current_d,current_t,tradeid,tradetype[tf->TradeType],tf->Num);
			user_Record(current_user->userInfo,TradeInfo);

			// 填充发送缓冲区
			sendlen = strlen(return_str) + 1;
			memcpy(sendbuffer, return_str, sendlen);
			sendcommand = 1;
		}
		else
		{
			strcpy((char*)sendbuffer, "下单失败");
			sendlen = strlen((char*)sendbuffer) + 1;
			sendcommand = 0;
		}
	}
}

void handle_userfunds(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到查询资金指令\n", dwThreadId);

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
			// 获得一个字符
			char nextchar = Result[i];

			if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
			{
				// 前一个字符串结束
				cols++;
				word[wordlen] = 0;

				wordlen = 0;

				if (rows == 2 && cols == 3) // 可用资金
				{							
					uf.TotalFunds = atof(word);
					printf("可用资金：%f ", atof(word));
				}
				if (rows == 2 && cols == 4) // 冻结资金
				{
					uf.LockFunds = atof(word);
					printf("冻结资金：%f ", atof(word));
				}
				if (rows == 2 && cols == 6) // 总资产
				{
					uf.BeginFunds = atof(word); // 模拟服务器使用开始资金
					printf("总资产：%f\n", atof(word));
				}
			}

			if (nextchar == '\n')
			{
				// 换行
				rows++;
				cols = 0;

			}
			else if (nextchar != '\t' && nextchar != 0)
			{
				word[wordlen] = nextchar;
				wordlen++;		
			}
		}

		printf("可用资金：%f 冻结资金：%f 开始资金：%f\n", uf.TotalFunds, uf.LockFunds, uf.BeginFunds);
		// 填充发送缓冲区
		sendlen = sizeof(VC_UserFunds);
		memcpy(sendbuffer, &uf, sendlen);
		sendcommand = VK_USER_FUNDS;
	}
	else
	{
		strcpy((char*)sendbuffer, "查询失败");
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 1;
	}
}


void handle_zijingufen(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到查询股份指令\n", dwThreadId);

	int ret = tdxquerydata(1, Result, ErrInfo);

	if (ret == 1)
	{
		// 返回股份信息
		struct VC_ZIJINGUFEN zjgf[100];
		memset(zjgf, 0, sizeof(VC_ZIJINGUFEN) * 100);

		int rows = 1;
		int cols = 0;
		int wordlen = 0;
		char word[100];
		for (unsigned long i = 0; i <= strlen(Result); i++)
		{
			// 获得一个字符
			char nextchar = Result[i];

			if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
			{
				// 前一个字符串结束
				cols++;
				word[wordlen] = 0;

				wordlen = 0;

				if (rows > 1)
				{
					if (cols == 1) // 证券代码
					{
						strcpy(zjgf[rows-2].stockcode, word);
					}
					if (cols == 2) // 证券名称
					{
						strcpy(zjgf[rows-2].stockname, word);
					}
					if (cols == 3) // 参考持股
					{
						zjgf[rows-2].num = atof(word); // 证券数量
					}
					if (cols == 4) // 可用持股
					{
						zjgf[rows-2].CansellNum = atof(word); // 可卖数量
					}
					if (cols == 5) // 成本价
					{
						zjgf[rows-2].buyPrice = atof(word);
					}
					if (cols == 6) // 当前价
					{
						zjgf[rows-2].NewPrice = atof(word);
					}
					if (cols == 8) // 最新市值
					{
						zjgf[rows-2].NewShizhi = atof(word);
					}
					if (cols == 9) // 浮动盈亏
					{
						zjgf[rows-2].FloatYk = atof(word);
					}
					if (cols == 10) // 盈亏比例
					{
						zjgf[rows-2].Ykbl = atof(word);
					}
				}						
			}
			if (nextchar == '\n')
			{
				// 换行
				rows++;
				cols = 0;

			}
			else if (nextchar != '\t' && nextchar != 0)
			{
				word[wordlen] = nextchar;
				wordlen++;		
			}
		}

		// 填充发送缓冲区
		if(rows > 1)
		{
			sendlen = sizeof(VC_ZIJINGUFEN) * (rows-1);
			memcpy(sendbuffer, zjgf, sendlen);
		}
		else
		{
			strcpy((char*)sendbuffer, "无记录");
			sendlen = strlen((char*)sendbuffer) + 1;
		}
		sendcommand = VK_ZIJINGUFEN;				
	}
	else
	{
		strcpy((char*)sendbuffer, "查询失败");
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 1;
	}
}

void handle_weituo(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到查询当日委托指令\n", dwThreadId);

	int ret = tdxquerydata(2, Result, ErrInfo);

	if (ret == 1)
	{
		// 返回当日委托信息
		struct VC_WEITUO weituo[100];
		memset(weituo, 0, sizeof(VC_WEITUO) * 100);

		int rows = 1;
		int cols = 0;
		int wordlen = 0;
		char word[100];
		for (unsigned long i = 0; i <= strlen(Result); i++)
		{
			// 获得一个字符
			char nextchar = Result[i];

			if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
			{
				// 前一个字符串结束
				cols++;
				word[wordlen] = 0;

				wordlen = 0;

				if (rows > 1)
				{
					if (cols == 3)	// 证券代码
					{
						strcpy(weituo[rows-2].stockcode, word);
					}
					if (cols == 4)	// 证券名称
					{
						strcpy(weituo[rows-2].stockname, word);
					}
					if (cols == 20)	// 委托编号
					{
						weituo[rows-2].id = atoi(word);
					}
					if (cols == 1)	// 委托时间
					{
						// 18:43:10 -> 184310
						char time[7];
						memcpy(time, word, 2);
						memcpy(&time[2], &word[3], 2);
						memcpy(&time[4], &word[6], 2);
						time[6] = '\0';
						weituo[rows-2].creaTime = atoi(time);
					}
					if (cols == 5) // 买卖标志: 0买，1卖
					{
						// 买卖标志：0买入, 1卖出, 2撤买入, 3撤卖出, 4银行转存, 5银行转取, 6股本分红, 7转增股本
						weituo[rows-2].trdeType = atoi(word);
					}
					if (cols == 8)	// 委托状态
					{
						// 交易状态：0未成交, 1已成, 2已撤买, 3已撤卖
						if (strcmp(word, "已撤") == 0)
						{
							if (weituo[rows-2].trdeType == 0) // 0买入标志
							{
								weituo[rows-2].trdeStatus = 2; 
							}
							else if (weituo[rows-2].trdeType == 1) // 1卖出标志
							{
								weituo[rows-2].trdeStatus = 3;
							}
						}
						else if (strcmp(word, "已成") == 0)
						{
							weituo[rows-2].trdeStatus = 1;
						}
						else
						{
							weituo[rows-2].trdeStatus = 0;
						}
					}
					if (cols == 9) // 委托价格
					{
						weituo[rows-2].price = atof(word);
					}
					if (cols == 10) // 委托数量
					{
						weituo[rows-2].num = atof(word);
					}
					if (cols == 11) // 成交价格
					{
						weituo[rows-2].turnoverPrice = atof(word);
					}
					if (cols == 12) // 成交数量
					{
						weituo[rows-2].turnoverNum = atof(word);
					}
					if (cols == 13) // 没有报价方式的信息
					{
						// 报价方式：0限价，1市价
						weituo[rows-2].reaPortType = 1; // 默认为市价
					}
				}						
			}
			if (nextchar == '\n')
			{
				// 换行
				rows++;
				cols = 0;

			}
			else if (nextchar != '\t' && nextchar != 0)
			{
				word[wordlen] = nextchar;
				wordlen++;		
			}
		}

		// 填充发送缓冲区
		if(rows > 1)
		{
			sendlen = sizeof(VC_WEITUO) * (rows-1);
			memcpy(sendbuffer, weituo, sendlen);
		}
		else
		{
			strcpy((char*)sendbuffer, "无记录");
			sendlen = strlen((char*)sendbuffer) + 1;
		}

		sendcommand = VK_WEITUO;
	}
	else
	{
		strcpy((char*)sendbuffer, "查询失败");
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 1;
	}
}
void handle_chengjiao(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到查询当日委托指令\n", dwThreadId);

	int ret = tdxquerydata(3, Result, ErrInfo);

	if (ret == 1)
	{

		// 返回当日成交信息
		struct VC_CHENGJIAO chengjiao[100];
		memset(chengjiao, 0, sizeof(VC_CHENGJIAO) * 100);

		int rows = 1;
		int cols = 0;
		int wordlen = 0;
		char word[100];
		for (unsigned long i = 0; i <= strlen(Result); i++)
		{
			// 获得一个字符
			char nextchar = Result[i];

			if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
			{
				// 前一个字符串结束
				cols++;
				word[wordlen] = 0;

				wordlen = 0;

				if (rows > 1)
				{
					if (cols == 3)	// 证券代码
					{
						strcpy(chengjiao[rows-2].stockcode, word);
					}
					if (cols == 4)	// 证券名称
					{
						strcpy(chengjiao[rows-2].stockname, word);
					}
					if (cols == 1)	// 成交时间
					{
						// 18:43:10 -> 184310
						char time[7];
						memcpy(time, word, 2);
						memcpy(&time[2], &word[3], 2);
						memcpy(&time[4], &word[6], 2);
						time[6] = '\0';
						chengjiao[rows-2].creatTime = atoi(time);
					}
					if (cols == 5) // 买卖标志: 0买，1卖
					{
						// 买卖标志：0买入, 1卖出, 2撤买入, 3撤卖出, 4银行转存, 5银行转取, 6股本分红, 7转增股本
						chengjiao[rows-2].trdeType = atoi(word);
					}
					if (cols == 12)	// 成交状态
					{
						// 交易状态：0未成交, 1已成, 2已撤买, 3已撤卖
						if (strcmp(word, "已撤") == 0)
						{
							if (chengjiao[rows-2].trdeType == 0) // 0买入标志
							{
								chengjiao[rows-2].trdeStatus = 2; 
							}
							else if (chengjiao[rows-2].trdeType == 1) // 1卖出标志
							{
								chengjiao[rows-2].trdeStatus = 3;
							}
						}
						else if (strcmp(word, "已成") == 0)
						{
							chengjiao[rows-2].trdeStatus = 1;
						}
						else
						{
							chengjiao[rows-2].trdeStatus = 0;
						}
					}
					if (cols == 16)	// 委托编号
					{
						chengjiao[rows-2].id = atoi(word);
					}
					if (cols == 8) // 成交价格
					{
						chengjiao[rows-2].turnoverPrice = atof(word);
					}
					if (cols == 9) // 成交数量
					{
						chengjiao[rows-2].turnoverNum = atof(word);
					}
					if (cols == 10) // 成交金额
					{
						chengjiao[rows-2].turnoverFunds = atof(word);
					}
				}						
			}
			if (nextchar == '\n')
			{
				// 换行
				rows++;
				cols = 0;

			}
			else if (nextchar != '\t' && nextchar != 0)
			{
				word[wordlen] = nextchar;
				wordlen++;		
			}
		}

		// 填充发送缓冲区
		if(rows > 1)
		{
			sendlen = sizeof(VC_CHENGJIAO) * (rows-1);
			memcpy(sendbuffer, chengjiao, sendlen);
		}
		else
		{
			strcpy((char*)sendbuffer, "无记录");
			sendlen = strlen((char*)sendbuffer) + 1;
		}

		sendcommand = VK_CHENGJIAO;
	}
	else
	{
		strcpy((char*)sendbuffer, "查询失败");
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 1;
	}
}
void handle_chedan(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到查询撤单委托指令\n", dwThreadId);

	int ret = tdxquerydata(4, Result, ErrInfo);

	if (ret == 1)
	{

		// 返回撤单信息
		struct VC_CHE_DAN chedan[100];
		memset(chedan, 0, sizeof(VC_CHE_DAN) * 100);

		int rows = 1;
		int cols = 0;
		int wordlen = 0;
		char word[100];
		for (unsigned long i = 0; i <= strlen(Result); i++)
		{
			// 获得一个字符
			char nextchar = Result[i];

			if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
			{
				// 前一个字符串结束
				cols++;
				word[wordlen] = 0;

				wordlen = 0;

				if (rows > 1)
				{
					if (cols == 3)	// 证券代码
					{
						strcpy(chedan[rows-2].stockcode, word);
					}
					if (cols == 4)	// 证券名称
					{
						strcpy(chedan[rows-2].stockname, word);
					}
					if (cols == 1)	// 委托时间
					{
						// 18:43:10 -> 184310
						char time[7];
						memcpy(time, word, 2);
						memcpy(&time[2], &word[3], 2);
						memcpy(&time[4], &word[6], 2);
						time[6] = '\0';
						chedan[rows-2].time = atoi(time);
					}
					if (cols == 5) // 买卖标志: 0买，1卖
					{
						// 买卖标志：0买入, 1卖出, 2撤买入, 3撤卖出, 4银行转存, 5银行转取, 6股本分红, 7转增股本
						chedan[rows-2].tradetag = atoi(word);
					}
					if (cols == 8)	// 委托状态
					{
						// 交易状态：0未成交, 1已成, 2已撤买, 3已撤卖
						if (strcmp(word, "已撤") == 0)
						{
							if (chedan[rows-2].tradetag == 0) // 0买入标志
							{
								chedan[rows-2].status = 2; 
							}
							else if (chedan[rows-2].tradetag == 1) // 1卖出标志
							{
								chedan[rows-2].status = 3;
							}
						}
						else if (strcmp(word, "已成") == 0)
						{
							chedan[rows-2].status = 1;
						}
						else
						{
							chedan[rows-2].status = 0;
						}
					}
					if (cols == 9) // 委托价格
					{
						chedan[rows-2].trdeprice = atof(word);
					}
					if (cols == 10) // 委托数量
					{
						chedan[rows-2].trdenum = atof(word);
					}
					if (cols == 2)	// 委托编号
					{
						chedan[rows-2].id = chedan[rows-2].tradeid = atoi(word);
						strcpy(chedan[rows-2].hashCode, word);
					}
					if (cols == 11) // 成交价格
					{
						chedan[rows-2].cjprice = atof(word);
					}
					if (cols == 12) // 成交数量
					{
						chedan[rows-2].cjnum = atof(word);
					}
					if (cols == 13) // 没有报价方式的信息
					{
						// 报价方式：0限价，1市价
						chedan[rows-2].reporttype = 1; // 默认为市价
					}
				}						
			}
			if (nextchar == '\n')
			{
				// 换行
				rows++;
				cols = 0;

			}
			else if (nextchar != '\t' && nextchar != 0)
			{
				word[wordlen] = nextchar;
				wordlen++;		
			}
		}

		// 填充发送缓冲区
		if(rows > 1)
		{
			sendlen = sizeof(VC_CHE_DAN) * (rows-1);
			memcpy(sendbuffer, chedan, sendlen);
		}
		else
		{
			strcpy((char*)sendbuffer, "无记录");
			sendlen = strlen((char*)sendbuffer) + 1;
		}

		sendcommand = VK_CHE_DAN;
	}
	else
	{
		strcpy((char*)sendbuffer, "查询失败");
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 1;
	}
}
void handle_chaxun5d(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到查询股票五档报价指令\n", dwThreadId);

	if(delen == sizeof(SockData))
	{
		// 获取股票代码
		struct SockData *sd = (struct SockData *)debuffer;
		printf("子线程%d: 待查询的股票代码为：%s\n", dwThreadId, sd->stockCode);

		int ret = tdxgetquote(sd->stockCode, Result, ErrInfo);
		if (ret == 1)
		{
			// 返回股票报价信息
			struct SockData return_sd;
			memset(&return_sd.Stock_data, 0, sizeof(tagRCV_REPORT_STRUCTEx));
			sd = &return_sd;

			int rows = 1;
			int cols = 0;
			int wordlen = 0;
			char word[100];
			for (unsigned long i = 0; i <= strlen(Result); i++)
			{
				// 获得一个字符
				char nextchar = Result[i];

				if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
				{
					// 前一个字符串结束
					cols++;
					word[wordlen] = 0;

					wordlen = 0;

					if (rows > 1)
					{
						if (cols == 1)	// 证券代码
						{
							strcpy(sd->Stock_data.m_szLabel, word);
						}
						if (cols == 2)	// 证券名称
						{
							strcpy(sd->Stock_data.m_szName, word);
						}
						if (cols == 3)	// 昨收
						{
							sd->Stock_data.m_fLastClose = atof(word);
						}
						if (cols == 4) // 今开
						{
							sd->Stock_data.m_fOpen = atof(word);
						}
						if (cols == 6)	// 当前价
						{
							sd->Stock_data.m_fNewPrice = atof(word);
						}
						if (cols == 7)	// 买一价
						{
							sd->Stock_data.m_fBuyPrice[0] = atof(word);
						}
						if (cols == 8)	// 买二价
						{
							sd->Stock_data.m_fBuyPrice[1] = atof(word);
						}
						if (cols == 9)	// 买三价
						{
							sd->Stock_data.m_fBuyPrice[2] = atof(word);
						}
						if (cols == 10)	// 买四价
						{
							sd->Stock_data.m_fBuyPrice4 = atof(word);
						}
						if (cols == 11)	// 买五价
						{
							sd->Stock_data.m_fBuyPrice5 = atof(word);
						}
						if (cols == 12)	// 买一量
						{
							sd->Stock_data.m_fBuyVolume[0] = atof(word);
						}
						if (cols == 13)	// 买二量
						{
							sd->Stock_data.m_fBuyVolume[1] = atof(word);
						}
						if (cols == 14)	// 买三量
						{
							sd->Stock_data.m_fBuyVolume[2] = atof(word);
						}
						if (cols == 15)	// 买四量
						{
							sd->Stock_data.m_fBuyVolume4 = atof(word);
						}
						if (cols == 16)	// 买五量
						{
							sd->Stock_data.m_fBuyVolume5 = atof(word);
						}
						if (cols == 17)	// 卖一价
						{
							sd->Stock_data.m_fSellPrice[0] = atof(word);
						}
						if (cols == 18)	// 卖二价
						{
							sd->Stock_data.m_fSellPrice[1] = atof(word);
						}
						if (cols == 19)	// 卖三价
						{
							sd->Stock_data.m_fSellPrice[2] = atof(word);
						}
						if (cols == 20)	// 卖四价
						{
							sd->Stock_data.m_fSellPrice4 = atof(word);
						}
						if (cols == 21)	// 卖五价
						{
							sd->Stock_data.m_fSellPrice5 = atof(word);
						}
						if (cols == 22)	// 卖一量
						{
							sd->Stock_data.m_fSellVolume[0] = atof(word);
						}
						if (cols == 23)	// 卖二量
						{
							sd->Stock_data.m_fSellVolume[1] = atof(word);
						}
						if (cols == 24)	// 卖三量
						{
							sd->Stock_data.m_fSellVolume[2] = atof(word);
						}
						if (cols == 25)	// 卖四量
						{
							sd->Stock_data.m_fSellVolume4 = atof(word);
						}
						if (cols == 26)	// 卖五量
						{
							sd->Stock_data.m_fSellVolume5 = atof(word);
						}
					}						
				}
				if (nextchar == '\n')
				{
					// 换行
					rows++;
					cols = 0;

				}
				else if (nextchar != '\t' && nextchar != 0)
				{
					word[wordlen] = nextchar;
					wordlen++;		
				}
			}

			// 填充发送缓冲区
			if(rows > 1)
			{
				sendlen = sizeof(SockData);
				memcpy(sendbuffer, &return_sd, sendlen);
			}
			else
			{
				strcpy((char*)sendbuffer, "无记录");
				sendlen = strlen((char*)sendbuffer) + 1;
			}

			sendcommand = STOCK_DATA;
		}
		else
		{
			strcpy((char*)sendbuffer, "查询失败");
			sendlen = strlen((char*)sendbuffer) + 1;
			sendcommand = 1;
		}


	}
}
void handle_weituolishi(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到委托历史查询指令\n", dwThreadId);

	if (delen == sizeof(Android_DateTimes))
	{
		// 获取查询开始日期和结束日期
		struct Android_DateTimes *ad = (struct Android_DateTimes *)debuffer;

		char start_date[10];
		char end_date[10];
		sprintf(start_date, "%04d%02d%02d", ad->StartYear, ad->StartMonth, ad->StartDay);
		sprintf(end_date, "%04d%02d%02d", ad->EndYear, ad->EndMonth, ad->EndDay);
		printf("子线程%d: 查询日期：%s -> %s\n", dwThreadId, start_date, end_date);

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
				// 获得一个字符
				char nextchar = Result[i];

				if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
				{
					// 前一个字符串结束
					cols++;
					word[wordlen] = 0;

					wordlen = 0;

					if (rows > 1)
					{
						if (cols == 4)	// 证券代码
						{
							strcpy(weituo[rows-2].stockcode, word);
						}
						if (cols == 5)	// 证券名称
						{
							strcpy(weituo[rows-2].stockname, word);
						}
						if (cols == 1)	// 委托日期
						{
							weituo[rows-2].Date = atoi(word);
						}
						if (cols == 2)	// 委托时间
						{
							// 18:43:10 -> 184310
							char time[7];
							memcpy(time, word, 2);
							memcpy(&time[2], &word[3], 2);
							memcpy(&time[4], &word[6], 2);
							time[6] = '\0';
							weituo[rows-2].Time = atoi(time);
						}
						if (cols == 6) // 买卖标志: 0买，1卖
						{
							// 买卖标志：0买入, 1卖出, 2撤买入, 3撤卖出, 4银行转存, 5银行转取, 6股本分红, 7转增股本
							weituo[rows-2].trdeType = atoi(word);
						}
						if (cols == 9)	// 委托状态
						{
							// 交易状态：0未成交, 1已成, 2已撤买, 3已撤卖
							if (strcmp(word, "已撤") == 0)
							{
								if (weituo[rows-2].trdeType == 0) // 0买入标志
								{
									weituo[rows-2].trdeStatus = 2; 
								}
								else if (weituo[rows-2].trdeType == 1) // 1卖出标志
								{
									weituo[rows-2].trdeStatus = 3;
								}
							}
							else if (strcmp(word, "已成") == 0)
							{
								weituo[rows-2].trdeStatus = 1;
							}
							else
							{
								weituo[rows-2].trdeStatus = 0;
							}
						}
						if (cols == 10) // 委托价格
						{
							weituo[rows-2].Eprice = atof(word);
						}
						if (cols == 11) // 委托数量
						{
							weituo[rows-2].Enum = atof(word);
						}
						if (cols == 12) // 成交价格
						{
							weituo[rows-2].Tprice = atof(word);
						}
						if (cols == 13) // 成交数量
						{
							weituo[rows-2].Tnum = atof(word);
						}
						if (cols == 19) // 委托编号
						{
							weituo[rows-2].Eno = atoi(word);
						}
						if (cols == 14) // 没有报价方式的信息
						{
							// 报价方式：0限价，1市价
							weituo[rows-2].reaPortType = 1; // 默认为市价
						}
					}						
				}
				if (nextchar == '\n')
				{
					// 换行
					rows++;
					cols = 0;

				}
				else if (nextchar != '\t' && nextchar != 0)
				{
					word[wordlen] = nextchar;
					wordlen++;		
				}
			}

			// 填充发送缓冲区
			if(rows > 1)
			{
				sendlen = sizeof(VC_WEITUO_LISHI) * (rows-1);
				memcpy(sendbuffer, weituo, sendlen);
			}
			else
			{
				strcpy((char*)sendbuffer, "无记录");
				sendlen = strlen((char*)sendbuffer) + 1;
			}

			sendcommand = VK_WEITUO_LISHI;
		}
		else
		{
			strcpy((char*)sendbuffer, "查询失败");
			sendlen = strlen((char*)sendbuffer) + 1;
			sendcommand = 1;
		}

	}
}
void handle_chengjiaolishi(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到成交历史查询指令\n", dwThreadId);

	if (delen == sizeof(Android_DateTimes))
	{
		// 获取查询开始日期和结束日期
		struct Android_DateTimes *ad = (struct Android_DateTimes *)debuffer;

		char start_date[10];
		char end_date[10];
		sprintf(start_date, "%04d%02d%02d", ad->StartYear, ad->StartMonth, ad->StartDay);
		sprintf(end_date, "%04d%02d%02d", ad->EndYear, ad->EndMonth, ad->EndDay);
		printf("子线程%d: 查询日期：%s -> %s\n", dwThreadId, start_date, end_date);

		int ret = tdxqueryhistory(1, start_date, end_date, Result, ErrInfo);

		if (ret == 1)
		{

			// 返回当日成交信息
			struct VC_CHENGJIO_LISHI chengjiao[1000];
			memset(chengjiao, 0, sizeof(VC_CHENGJIO_LISHI) * 1000);

			int rows = 1;
			int cols = 0;
			int wordlen = 0;
			char word[100];
			for (unsigned long i = 0; i <= strlen(Result); i++)
			{
				// 获得一个字符
				char nextchar = Result[i];

				if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
				{
					// 前一个字符串结束
					cols++;
					word[wordlen] = 0;

					wordlen = 0;

					if (rows > 1)
					{
						if (cols == 4)	// 证券代码
						{
							strcpy(chengjiao[rows-2].stockcode, word);
						}
						if (cols == 5)	// 证券名称
						{
							strcpy(chengjiao[rows-2].stockname, word);
						}
						if (cols == 1)	// 成交日期
						{									
							chengjiao[rows-2].Date = atoi(word);
						}
						if (cols == 2)	// 成交时间
						{
							// 18:43:10 -> 184310
							char time[7];
							memcpy(time, word, 2);
							memcpy(&time[2], &word[3], 2);
							memcpy(&time[4], &word[6], 2);
							time[6] = '\0';
							chengjiao[rows-2].Time = atoi(time);
						}
						if (cols == 6) // 买卖标志: 0买，1卖
						{
							// 买卖标志：0买入, 1卖出, 2撤买入, 3撤卖出, 4银行转存, 5银行转取, 6股本分红, 7转增股本
							chengjiao[rows-2].trdeType = atoi(word);
						}
						if (cols == 8) // 成交价格
						{
							chengjiao[rows-2].Tprice = atof(word);
						}
						if (cols == 9) // 成交数量
						{
							chengjiao[rows-2].Tnum = atof(word);
						}
						if (cols == 12) // 委托编号
						{
							chengjiao[rows-2].Tno = atoi(word);
						}
					}						
				}
				if (nextchar == '\n')
				{
					// 换行
					rows++;
					cols = 0;

				}
				else if (nextchar != '\t' && nextchar != 0)
				{
					word[wordlen] = nextchar;
					wordlen++;		
				}
			}

			// 填充发送缓冲区
			if(rows > 1)
			{
				sendlen = sizeof(VC_CHENGJIO_LISHI) * (rows-1);
				memcpy(sendbuffer, chengjiao, sendlen);
			}
			else
			{
				strcpy((char*)sendbuffer, "无记录");
				sendlen = strlen((char*)sendbuffer) + 1;
			}

			sendcommand = VK_CHENGJIO_LISHI;
		}
		else
		{
			strcpy((char*)sendbuffer, "查询失败");
			sendlen = strlen((char*)sendbuffer) + 1;
			sendcommand = 1;
		}
	}
}
void handle_zijinliushui(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到资金历史查询指令\n", dwThreadId);

	if (delen == sizeof(Android_DateTimes))
	{
		// 获取查询开始日期和结束日期
		struct Android_DateTimes *ad = (struct Android_DateTimes *)debuffer;

		char start_date[10];
		char end_date[10];
		sprintf(start_date, "%04d%02d%02d", ad->StartYear, ad->StartMonth, ad->StartDay);
		sprintf(end_date, "%04d%02d%02d", ad->EndYear, ad->EndMonth, ad->EndDay);
		printf("子线程%d: 查询日期：%s -> %s\n", dwThreadId, start_date, end_date);

		// 获取资金历史列表
		int ret = tdxqueryhistory(3, start_date, end_date, Result, ErrInfo);

		if (ret == 1)
		{

			// 返回当日成交信息
			struct VC_ZIJINLIUSHUI zjls[1000];
			memset(zjls, 0, sizeof(VC_ZIJINLIUSHUI) * 1000);

			int rows = 1;
			int cols = 0;
			int wordlen = 0;
			char word[100];
			for (unsigned long i = 0; i <= strlen(Result); i++)
			{
				// 获得一个字符
				char nextchar = Result[i];

				if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
				{
					// 前一个字符串结束
					cols++;
					word[wordlen] = 0;

					wordlen = 0;

					if (rows > 1)
					{
						if (cols == 4)	// 证券代码
						{
							strcpy(zjls[rows-2].stockcode, word);
						}
						if (cols == 5)	// 证券名称
						{
							strcpy(zjls[rows-2].stockname, word);
						}
						if (cols == 1)	// 成交日期
						{									
							zjls[rows-2].Date = atoi(word);
						}
						if (cols == 2)	// 成交时间
						{
							// 18:43:10 -> 184310
							char time[7];
							memcpy(time, word, 2);
							memcpy(&time[2], &word[3], 2);
							memcpy(&time[4], &word[6], 2);
							time[6] = '\0';
							zjls[rows-2].Time = atoi(time);
						}
						if (cols == 6) // 成交价格
						{
							zjls[rows-2].price = atof(word);
						}
						if (cols == 7) // 成交数量
						{
							zjls[rows-2].num = atof(word);
						}
						if (cols == 16) // 发生金额
						{
							zjls[rows-2].funds = atof(word);
						}
						if (cols == 17) // 资金本次余额
						{
							zjls[rows-2].canusefunds = atof(word);
						}
					}						
				}
				if (nextchar == '\n')
				{
					// 换行
					rows++;
					cols = 0;

				}
				else if (nextchar != '\t' && nextchar != 0)
				{
					word[wordlen] = nextchar;
					wordlen++;		
				}
			}

			// 填充发送缓冲区
			if(rows > 1)
			{
				sendlen = sizeof(VC_ZIJINLIUSHUI) * (rows-1);
				memcpy(sendbuffer, zjls, sendlen);
			}
			else
			{
				strcpy((char*)sendbuffer, "无记录");
				sendlen = strlen((char*)sendbuffer) + 1;
			}

			sendcommand = VK_ZIJINLIUSHUI;
		}
		else
		{
			strcpy((char*)sendbuffer, "查询失败");
			sendlen = strlen((char*)sendbuffer) + 1;
			sendcommand = 1;
		}
	}
}
void handle_jiaogedan(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo)
{
	printf("子线程%d: 接收到交割单历史查询指令\n", dwThreadId);

	if (delen == sizeof(Android_DateTimes))
	{
		// 获取查询开始日期和结束日期
		struct Android_DateTimes *ad = (struct Android_DateTimes *)debuffer;

		char start_date[10];
		char end_date[10];
		sprintf(start_date, "%04d%02d%02d", ad->StartYear, ad->StartMonth, ad->StartDay);
		sprintf(end_date, "%04d%02d%02d", ad->EndYear, ad->EndMonth, ad->EndDay);
		printf("子线程%d: 查询日期：%s -> %s\n", dwThreadId, start_date, end_date);

		// 获取交割单历史列表

		int ret = tdxqueryhistory(2, start_date, end_date, Result, ErrInfo);

		if (ret == 1)
		{

			// 返回当日成交信息
			struct VC_JIAOGEDAN jgd[1000];
			memset(jgd, 0, sizeof(VC_JIAOGEDAN) * 1000);

			int rows = 1;
			int cols = 0;
			int wordlen = 0;
			char word[100];
			for (unsigned long i = 0; i <= strlen(Result); i++)
			{
				// 获得一个字符
				char nextchar = Result[i];

				if (wordlen > 0 && (nextchar == '\t' || nextchar == '\n' || nextchar == 0))
				{
					// 前一个字符串结束
					cols++;
					word[wordlen] = 0;

					wordlen = 0;

					if (rows > 1)
					{
						if (cols == 2)	// 证券代码
						{
							strcpy(jgd[rows-2].stockcode, word);
						}
						if (cols == 3)	// 证券名称
						{
							strcpy(jgd[rows-2].stockname, word);
						}
						if (cols == 1)	// 成交日期
						{									
							jgd[rows-2].Date = atoi(word);
						}
						if (cols == 16)	// 成交时间
						{
							// 18:43:10 -> 184310
							char time[7];
							memcpy(time, word, 2);
							memcpy(&time[2], &word[3], 2);
							memcpy(&time[4], &word[6], 2);
							time[6] = '\0';
							jgd[rows-2].Time = atoi(time);
						}
						if (cols == 5) // 成交价格
						{
							jgd[rows-2].Tprice = atof(word);
						}
						if (cols == 6) // 成交数量
						{
							jgd[rows-2].Tnum = atof(word);
						}
						if (cols == 7) // 成交金额
						{
							jgd[rows-2].TFunds = atof(word);
						}
						if (cols == 7) // 清算金额
						{
							jgd[rows-2].Funds = atof(word); // 发生金额
						}
						if (cols == 8) // 手续费
						{
							jgd[rows-2].Sxfee = atof(word); // 佣金
						}
						if (cols == 10) // 过户费
						{
							jgd[rows-2].Ghfee = atof(word); // 过户费
						}
						if (cols == 9) // 印花税
						{
							jgd[rows-2].Yhfee = atof(word); // 印花税
						}
						if (cols == 11) // 附加费
						{
							jgd[rows-2].Qtfee = atof(word); // 其他费
						}
						if (cols == 15) // 委托编号
						{
							jgd[rows-2].Tno = atoi(word);
						}
					}						
				}
				if (nextchar == '\n')
				{
					// 换行
					rows++;
					cols = 0;

				}
				else if (nextchar != '\t' && nextchar != 0)
				{
					word[wordlen] = nextchar;
					wordlen++;		
				}
			}

			// 填充发送缓冲区
			if(rows > 1)
			{
				sendlen = sizeof(VC_JIAOGEDAN) * (rows-1);
				memcpy(sendbuffer, jgd, sendlen);
			}
			else
			{
				strcpy((char*)sendbuffer, "无记录");
				sendlen = strlen((char*)sendbuffer) + 1;
			}

			sendcommand = VK_JIAOGEDAN;
		}
		else
		{
			strcpy((char*)sendbuffer, "查询失败");
			sendlen = strlen((char*)sendbuffer) + 1;
			sendcommand = 1;
		}
	}
}

void handle_chedanorder(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId, char *Result, char *ErrInfo,UserList current_user)
{
	printf("子线程%d: 接收到撤单指令\n", dwThreadId);
	debuffer[delen] = 0;
	printf("子线程%d: TradeID = %s\n", dwThreadId, debuffer);

	// 撤单结果
	int ret = tdxcancelorder((char*)debuffer, 1, Result, ErrInfo);
	//int ret=1;
	if (ret == 1)
	{
		sendlen = strlen(Result) + 1;
		memcpy(sendbuffer, Result, sendlen);
		sendcommand = 1;
		//保存用户撤单信息
		char current_t[20],current_d[20];
		user_current_time(current_d,current_t);
		char TradeInfo[512];
		//sprintf(TradeInfo,"%s%s %s %s %s %d","\n",current_d,current_t,"klklklih0i99","撤",0);
		sprintf(TradeInfo,"%s%s %s %s %s %d","\n",current_d,current_t,debuffer,"撤",0);
		user_Record(current_user->userInfo,TradeInfo);

	}
	else
	{
		strcpy((char*)sendbuffer, "撤单失败");
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 0;
	}
}
int handle_msg(int command, char *debuffer, int delen, char *sendbuffer, int &sendlen, int &sendcommand, int dwThreadId,UserList *current_user)
{

	char *Result = new char[10*1024*1024];
	char *ErrInfo = new char[1024*1024];

	// 处理消息
	printf("子线程%d: 开始处理消息...\n\n", dwThreadId);
	if (command == VK_LOGIN)//登录时需要验证信息
	{
		handle_login(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo,current_user);
	}
	else if (command == BUY)//下单成功时需要记录信息
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
		handle_chedanorder(command, debuffer, delen, sendbuffer, sendlen, sendcommand, dwThreadId, Result, ErrInfo,*current_user);
	}
	else
	{
		strcpy((char*)sendbuffer, "你好，TCP客户端，我是服务端!\n");
		printf("子线程%d: 返回请求结果：%s\n", dwThreadId, sendbuffer);
		sendlen = strlen((char*)sendbuffer) + 1;
		sendcommand = 0;
	}

	delete []Result;
	delete []ErrInfo;

	return 1;
}