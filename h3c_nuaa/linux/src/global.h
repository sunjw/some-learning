//ThorClient 802.1x (using WinPcap 3.2 alpha1)
//Download: http://thorqq.ys168.com 
//E-mail:   thorqq@163.com 
//Author:   Thorqq
/////////////////////////////////////////////////////////////

#ifndef _GLOBAL_H
#define _GLOBAL_H
#include <pcap.h>
#define FILENAME "ThorClient.dat"

#define WM_DOT1X	       WM_USER + 1006//改变认证状态提示

//EAP_HEAD  CODE
#define EAP_REQUEST		0x01
#define EAP_RESPONSE	0x02
#define EAP_SUCCESS		0x03
#define EAP_FAILURE		0x04
#define EAP_OTHER		0x0a
//EAP DATA	TYPE
#define EAP_IDENTIFY		0x01
#define EAP_NOTIFICATION	0x02
#define EAP_MD5				0X04
#define EAP_LOGOUT			0X08
#define EAP_ERROR			0X09
#define EAP_KEEPONLINE		0X14

typedef unsigned char  u_char8;
typedef unsigned char  u_char;
typedef unsigned short u_short16;
typedef unsigned short u_short;

#define MAXINTERFACES   16

#include <string>

struct InterfaceInfo
{
	std::string name ;
	std::string ipaddr;
	int macaddr[6];
};

//防止为了字节对齐而在结构体中插入0
#pragma pack(push) // 将当前pack设置压栈保存
#pragma pack(1)// 必须在结构体定义之前使用

typedef struct USERDATA
{
	char username[1024];
	char password[1024];
	u_char8 ip[4];
	u_char8 mac[6];
	char nic[60];
	char nicdes[60];

	char morb;		//广播还是多播触发认证
	char dhcp;		//自动获取IP地址
	char updateip;	//上传IP
	char multisend;	//多播发送数据帧
	char relogin;	//断线90s后自动重新连接

	char autologin;	//启动自动连接
	char rempwd;	//记住密码
}USERDATA, *PUSERDATA;

typedef struct ETHHDR
{
	u_char8	DestMAC[6];
	u_char8	SourMAC[6];
	u_short	EthType;
}ETHHDR, *PETHHDR;

typedef struct PKTHDR
{
	u_char8	DestMAC[6];
	u_char8	SourMAC[6];
	u_char8 EthType[2];

	u_char8 Version;
	u_char8	PktType;
	u_short Len1;
	
	u_char8	Code;
	u_char8	Id;
	u_short Len2;

	u_char8 EapType;
}PKTHDR, *PPKTHDR, LOGINFRM, *PLOGINFRM, LOGOUTFRM, *PLOGOUTFRM;

typedef struct VERSIONFRM
{
	PKTHDR	Hdr;
	u_char8 Version[50];
}VERSIONFRM, *PVERSIONFRM;

typedef struct USERNAMEFRM
{
	PKTHDR	Hdr;
	u_char8 Unknown[2];
	u_char8 Ip[4];
	u_char8 Username[50];
}USERNAMEFRM, *PUSERNAMEFRM;

typedef struct PASSWORDFRM
{
	PKTHDR	Hdr;
	u_char8 Unknown[1];
	u_char8 Md5Pwd[16];
	u_char8 Username[50];
}PASSWORDFRM, *PPASSWORDFRM;

typedef struct KEEPONLINEFRM
{
	PKTHDR	Hdr;
	u_char8 Unknown[3];
	u_char8 Ip[4];
	u_char8 Username[50];
}KEEPONLINEFRM, *PKEEPONLINEFRM;

#pragma pack(pop) // 恢复先前的pack设置

#endif

