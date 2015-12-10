#include "config.h"



void get_config(char *gddm1, char *gddm2, char *gdmc, char *server_ip, char *account, char *jypass, char *txpass, int *port)
{
	char *path = new char[200];
	GetCurrentDirectory(256, path);//获得配置文件绝对目录
	char configpath[250];
	int namelen = 100;
	sprintf_s(configpath, "%s%s%s", path, "\\", "config.ini");
	GetPrivateProfileString("config", "gddm1", "", gddm1, namelen, configpath); //没有该值时默认为空
	GetPrivateProfileString("config", "gddm2", "", gddm2, namelen, configpath);
	GetPrivateProfileString("config", "gdmc", "", gdmc, namelen, configpath);
	GetPrivateProfileString("config", "ip", "", server_ip, namelen, configpath);
	GetPrivateProfileString("config", "account", "", account, namelen, configpath);
	GetPrivateProfileString("config", "jypass", "", jypass, namelen, configpath);
	GetPrivateProfileString("config", "txpass", "", txpass, namelen, configpath);
	*port = (int)GetPrivateProfileInt("config", "port", 0, configpath);

	delete[] path;
}
/*
lpAppName：字段值
lpKeyName:键名
len:分配的内存大小,需要在外部delete
return：键值
*/
char *get_config(char *lpKeyName, const int len, char* lpAppName)
{
	char *lpString = new char[len];
	char *path = new char[200];
	GetCurrentDirectory(256, path);//获得配置文件绝对目录
	char configpath[250];
	sprintf_s(configpath, "%s%s%s", path, "\\", "config.ini");
	GetPrivateProfileString(lpAppName, lpKeyName, "", lpString, len, configpath);
	delete[] path;
	return lpString;
}