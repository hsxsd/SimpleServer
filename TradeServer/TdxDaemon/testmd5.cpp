// testmd5.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "md5.h"
#include <string.h>


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

int demd55(char *in, char *out)
{
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char in[100] = "123456";
	char out[100];
	int p1 = md55(in, out);
	printf("%s -> %s\n", in, out);
	return 0;
}

