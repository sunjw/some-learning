// HuaweiNetwork.cpp: implementation of the HuaweiNetwork class.
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <ctime>
#include <cstring>

#include <netinet/in.h>
#include <pcap.h>

#include "HuaweiNetwork.h"

using namespace std;

HuaweiNetwork::HuaweiNetwork(const string& username,
							 const string& passwd,
							 const string& interfacename,
							 int* ip,
							 int* mac,
							 const string& version)
							 :m_fp(NULL)
{
	//USERDATA * m_pData = &m_Data;
	strcpy(m_Data.username, username.c_str());
	strcpy(m_Data.password, passwd.c_str());
	strcpy(m_Data.nic, interfacename.c_str());

	for(int i = 0; i < 4; ++i)
		m_Data.ip[i] = ip[i];
	/*m_Data.ip[0] = ip[0];
	m_Data.ip[1] = ip[1];
	m_Data.ip[2] = ip[2];
	m_Data.ip[3] = ip[3];*/

	for(int i = 0; i < 6; ++i)
		m_Data.mac[i] = mac[i];
	//m_Data.mac[0] = mac[0];
	//m_Data.mac[1] = mac[1];
	//m_Data.mac[2] = mac[2];
	//m_Data.mac[3] = mac[3];
	//m_Data.mac[4] = mac[4];
	//m_Data.mac[5] = mac[5];

	m_Data.morb	= 0;
	m_Data.dhcp = 1;
	m_Data.multisend = 0;
	m_Data.updateip = 1;
	m_Data.relogin = 0;

	//u_char mcast[6] = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x03};
	//u_char mcast[6] = {0x00, 0x0f, 0xe2, 0x0f, 0xfd, 0xde};
	u_char mcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	memcpy(m_DestMac, mcast, 6);
	string cver = "CH V" + version;
	memcpy(m_ClientVersion, cver.c_str(), 14);
	srand(time(0));
}

HuaweiNetwork::~HuaweiNetwork()
{}

void HuaweiNetwork::MainLoop()
{
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	int res;
	time_t dwTick, dwOldTick;
	dwTick = dwOldTick = clock();
	int flag = 0;


	while((res = pcap_next_ex(m_fp, &header, &pkt_data)) >= 0)
	{

		if(res == 0)// Timeout elapsed
		{
			dwTick = clock();
			if(dwTick - dwOldTick >= 30000 //ms
				&& m_Data.relogin == 1)
			{
				//cout << "Reconnecting...";
				//char *t="重新连接...";
				//::PostMessage(hWnd,WM_INFO,NULL,(LPARAM)"重新连接...");
				Message(WM_INFO, "重新连接...");
				dwOldTick = clock();
			}
			else if(dwTick - dwOldTick >= 90000)
			{
				//char *t="连接失败...";
				//::PostMessage(hWnd,WM_INFO,NULL,(LPARAM)"连接失败...");
				Message(WM_INFO, "连接失败...");
				ConnectionInterrupted();
				break;
			}
			continue;
		}
		dwOldTick = clock();

		PPKTHDR pbuf = (PPKTHDR)pkt_data;
		memcpy(m_DestMac, pbuf->SourMAC, 6);

		if(pbuf->Code == EAP_REQUEST)
		{
			switch(pbuf->EapType)
			{
			case EAP_KEEPONLINE:

				SendKeeponline(pbuf->Id);
				dwOldTick = clock();

				break;
			case EAP_NOTIFICATION:

				SendVersion(pbuf->Id);
				break;
			case EAP_IDENTIFY:
				SendUsername(pbuf->Id);
				break;
			case EAP_MD5:
				SendPassword(pbuf->Id, ((PPASSWORDFRM) pkt_data)->Md5Pwd);
				break;
			}

			continue;
		}

		if(pbuf->Code == EAP_RESPONSE)
		{
			//char *t="Response";
			//::PostMessage(hWnd,WM_INFO,NULL,(LPARAM)"Response");
			Message(WM_INFO, "Response");
			continue;
		}

		if(pbuf->Code == EAP_SUCCESS)
		{

			//cout << "Successfully Login";
			//char *t="登录成功！";
			//::PostMessage(hWnd,WM_INFO,NULL,(LPARAM)"登录成功！");
			Message(WM_INFO, "登录成功！");
			Connected();
			continue;
		}

		if(pbuf->Code == EAP_FAILURE)
		{

			if(pbuf->EapType == EAP_LOGOUT)
			{
				//cout << "Logout!";
				//char *t="Logout!";
				//::PostMessage(hWnd,WM_INFO,NULL,(LPARAM)"Logout!");
				Message(WM_INFO, "Logout!");
			}
			else
			{
				// Network Failure;
				//puts((const char *)(pkt_data+0x18));
				//::PostMessage(hWnd,WM_INFO,NULL,(LPARAM)pkt_data+0x18);
				Message(WM_INFO, "认证失败");
			}
			ConnectionInterrupted();
			break;//jump out of the loop
		}

		if(pbuf->Code == EAP_OTHER)
		{
			if(flag == 0)
			{
				flag = 1;
				continue;
			}
			u_char a[0xff] = {0};
			memcpy(a, pkt_data+0x1a, *(pkt_data +0x11)-4);
			//			a[*(pkt_data +0x11)-3] = '\0';
			for(int i= 1; i<0xff; i++)
			{
				if(a[0xff-i] == 0x3D)
				{
					a[0xff-i]='\0';
					//a[0xff-i] = '\t';
					//a[0xff-i+1] = '\n';
					break;
				}
			}
			//cout << (const char *)a;
			//::PostMessage(hWnd,WM_INFO,NULL,(LPARAM)a);
			Message(WM_INFO, (const char*)a);

			continue;
		}
	}
	CloseAdapter();
}

void HuaweiNetwork::Connect()
{
	//puts("Initializing Network Adapter...");
	//::PostMessage(hWnd, WM_INFO, NULL, (LPARAM)"初始化网络适配器...");
	Message(WM_INFO, "初始化网络适配器...");
	if(!OpenAdapter())
	{
		//puts("Network Adapter Initializing Failed.");
		//::PostMessage(hWnd,WM_INFO,NULL,
			//(LPARAM)"网络适配器初始化失败");
		Message(WM_INTERRUPTED, "网络适配器初始化失败");
		return;
	}

	// FIXME : USE THIS without qt thread
	// ORIG: start(QThread::HighPriority);
	if(SendHello())
	{
		MainLoop();
		//this->run();
	}
	//IPRenew(false);
	// \fixme

	// arak 1- don't know what this todo
	//SendLogin();

	//		Connected();

	//	m_hThread = ::CreateThread(NULL, 0, WorkProc, this, 0, NULL);



}

void HuaweiNetwork::DisConnect()
{
	//		ConnectionInterrupted();
	if(!m_fp)
		OpenAdapter();

	if(m_fp)
	{
		SendLogout();
		//WinExec("ipconfig /release",SW_HIDE);
		IPRenew(true);

		//puts("Disconnected.");
		//char *t="连接断开.";
		//::PostMessage(hWnd,WM_INFO,NULL,(LPARAM)"连接断开");
		Message(WM_INFO, "连接断开");
	}
}

void HuaweiNetwork::Connected()
{
	//system("dhclient");
	//char *t="刷新IP地址...";
	//::PostMessage(hWnd,WM_INFO,NULL,
		//(LPARAM)"刷新 IP 地址...");
	Message(WM_INFO, "刷新 IP 地址...");
	//system("ipconfig /renew");
	//WinExec("ipconfig /renew",SW_HIDE);

	IPRenew(false);

}

void HuaweiNetwork::ConnectionInterrupted()
{
	//puts("todo : finish this: HuaweiNetwork::ConnectionInterrupted");
	IPRenew(true);
	//::PostMessage(hWnd,WM_INTERRUPTED,NULL,NULL);
	Message(WM_INTERRUPTED, NULL);
}

bool  HuaweiNetwork::OpenAdapter()
{
	bpf_u_int32 netmask = 0;
	char pcap_filter[100];              //filter space
	struct bpf_program pcap_fp;         //hold the compiled filter.
	char errbuf[PCAP_ERRBUF_SIZE] = "";
	if(!(m_fp = pcap_open_live(m_Data.nic,// name of the device
		256,		// portion of the packet to capture, max 65536
		0,			// promiscuous mode closed
		10,			// read timeout
		errbuf)))	// error buffer
	{
		//		AfxMessageBox(errbuf);
		puts((const char *)errbuf);
		return false;
	}
	sprintf(pcap_filter, "ether dst %x:%x:%x:%x:%x:%x and ether proto 0x888e",
		m_Data.mac[0],m_Data.mac[1],m_Data.mac[2],
		m_Data.mac[3],m_Data.mac[4],m_Data.mac[5]);
	//	sprintf(pcap_filter, " or ether dst 01:80:c2:00:00:03 and ether proto 0x888e",
	//					  m_pData->Mac[0],m_pData->Mac[1],m_pData->Mac[2],
	//					  m_pData->Mac[3],m_pData->Mac[4],m_pData->Mac[5]);

	if (pcap_compile(m_fp, &pcap_fp, pcap_filter, 0, netmask) == -1)
		return false;

	if (pcap_setfilter(m_fp, &pcap_fp) == -1)
		return false;

	return true;
}

void HuaweiNetwork::CloseAdapter()
{
	if(m_fp != 0)
	{
		pcap_close(m_fp);
		m_fp = 0;
	}
}

void HuaweiNetwork::InitBuf(u_char *buf)
{
	u_char prototype[3] = {0x88, 0x8e, 0x01};

	if(m_Data.multisend == 0x00)	 //set destination MAC
		memcpy(buf, m_DestMac, 6);

	memcpy(buf+6 , m_Data.mac, 6); //set source MAC
	memcpy(buf+12, prototype,    3); //set protocol type and its version
}

bool HuaweiNetwork::SendHello()
{
	u_char buf[100] = {	0 };
	PLOGINFRM pbuf = (PLOGINFRM) buf;

	InitBuf(buf);//Set Dest MAC, Source MAC, Protocol Type and Version
	if(m_Data.morb == 'b')
		memset(buf, 0xff, 6);
	pbuf->PktType = 0x01;		 //EAPOL-START

	if(!::pcap_sendpacket(m_fp, buf, 60))
	{
		//puts("Finding Connection Device...");
		//::PostMessage(hWnd, WM_INFO, NULL,
			//(LPARAM)"查找连接设备...");
		Message(WM_INFO, "查找连接设备...");
		return true;
	}
	else
	{
		//::PostMessage(hWnd, WM_INFO, NULL,
			//(LPARAM)"查找连接设备失败...");
		Message(WM_INTERRUPTED, "查找连接设备失败...");
		return false;
	}
}

bool HuaweiNetwork::SendLogout()
{
	u_char buf[100] = {	0 };
	PLOGOUTFRM pbuf = (PLOGOUTFRM) buf;

	InitBuf(buf);//set Dest MAC, Source MAC, Protocol Type and Version
	pbuf->PktType = 0x02;//EAPOL-LOGOUT

	if(!::pcap_sendpacket(m_fp, buf, 60))
	{
		//puts("Disconnecting ...");
		//::PostMessage(hWnd,WM_INFO,NULL,
			//(LPARAM)"正在断开...");
		Message(WM_INFO, "正在断开...");
		return true;
	}

	return false;
}

bool HuaweiNetwork::SendVersion(const u_char Id)
{
	/*	u_char buf[100] = {

	0x2e, 0x25, 0x4d, 0x3b, 0x5f, 0x43, 0x5f,
	0x5d, 0x40, 0x5d, 0x5f, 0x5e, 0x5c, 0x6d, 0x6d,
	0x6d, 0x6d, 0x6d, 0x6d, 0x6d,
	*//*
	u_char buf[100] = {
	0x01, 0x80, 0xc2, 0x00, 0x00, 0x03, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x88, 0x8e, 0x01, 0x00,
	0x00, 0x31, 0x02, 0x01 ,0x00, 0x31, 0x02, 0x01,
	0x16, // 25 bytes
	//// version[2] = buf[25]
	0x1d, 0x09, // 27 bytes
	////
	0x2b, 0x63, 0x07, 0x25, 0x75,
	0x6e, 0x6f, 0x4b, 0x4e, 0x24, 0x39, 0x45, 0x0f,
	0x67, 0xd5, 0xa8, 0x3d, 0x0b, // 27+18=45

	0x02, 0x16, 0x3a, // 45 +3 = 48
	0x71, 0x38, 0x01, 0x0b, 0x3b, 0x7e, 0x3d, 0x26,
	0x7c, 0x7c, 0x17, 0x0b, 0x46, 0x08, 0x32, 0x32, // 48 + 16 = 64
	0x08, 0x46, 0x0b // 64 + 3 = 67
	};
	*/
	//	PVERSIONFRM pbuf = (PVERSIONFRM) m_buf;

	// 
	u_char buf[100] = {0};
	PVERSIONFRM pbuf = (PVERSIONFRM) buf;
	InitBuf(buf);
	pbuf->Hdr.PktType = 0x00;
	pbuf->Hdr.Len1    = htons(0x31);
	pbuf->Hdr.Code    = 0x02;
	pbuf->Hdr.Id      = 0x01;
	pbuf->Hdr.Len2    = pbuf->Hdr.Len1;
	pbuf->Hdr.EapType = 0x02;
	GenerateVersion(pbuf->Version);

	InitBuf(buf);
	if(!::pcap_sendpacket(m_fp, buf, 67))
	{
		//puts("Finding Connection Device...");
		//::PostMessage(hWnd,WM_INFO,NULL,
			//(LPARAM)"查找连接设备...");
		Message(WM_INFO, "查找连接设备...");
		return true;
	}

	return false;
}
//	#include <sys/socket.h>
bool HuaweiNetwork::SendUsername(const u_char Id)
{
	u_char buf[100] = { 0 };
	PUSERNAMEFRM pbuf = (PUSERNAMEFRM) buf;

	InitBuf(buf);
	pbuf->Hdr.Len1 = htons(static_cast<u_short>(strlen(m_Data.username) + 0x0b));
	pbuf->Hdr.Code = 0x02;
	pbuf->Hdr.Id = Id;
	pbuf->Hdr.Len2 = pbuf->Hdr.Len1;
	pbuf->Hdr.EapType = 0x01;
	pbuf->Unknown[0] = 0x15;
	pbuf->Unknown[1] = 0x04;
	if(m_Data.updateip == 0x01)
		memcpy(pbuf->Ip, m_Data.ip, 4);
	memcpy(&pbuf->Username, &m_Data.username, strlen(m_Data.username));

	if(!::pcap_sendpacket(m_fp, buf, 60))
	{
		//puts("Verifying User Name ...");
		//char *t="Verifying User Name ...";
		//::PostMessage(hWnd,WM_INFO,NULL,(LPARAM)t);
		return true;
	}

	return false;
}

bool HuaweiNetwork::SendPassword(const u_char Id, const u_char *Chap)
{
	u_char buf[100] = { 0 };
	PPASSWORDFRM pbuf = (PPASSWORDFRM) buf;

	InitBuf(buf);
	pbuf->Hdr.Len1 = htons(static_cast<u_short>(strlen(m_Data.username) + 0x16));
	pbuf->Hdr.Code = 0x02;
	pbuf->Hdr.Id = Id;
	pbuf->Hdr.Len2 = pbuf->Hdr.Len1;
	pbuf->Hdr.EapType = 0x04;
	pbuf->Unknown[0] = 0x10;
	SetMd5Buf(pbuf, Id, Chap);
	memcpy(pbuf->Username, m_Data.username, strlen(m_Data.username));

	if(!::pcap_sendpacket(m_fp, buf, 60))
	{
		//puts("Verifying Password ...");
		//msg = "验证用户名 ...";
		//::PostMessage(hWnd,WM_INFO,NULL,(LPARAM)"验证用户名...");
		Message(WM_INFO, "验证用户名...");
		//msg = "验证密码 ...";
		//::PostMessage(hWnd,WM_INFO,NULL,(LPARAM)"验证密码...");
		Message(WM_INFO, "验证密码...");
		return true;
	}

	return false;
}

bool HuaweiNetwork::SendKeeponline(const u_char Id)
{
	u_char buf[100] = { 0 };
	PKEEPONLINEFRM pbuf = (PKEEPONLINEFRM) buf;

	InitBuf(buf);
	pbuf->Hdr.Len1 = htons(static_cast<u_short>(strlen(m_Data.username) + 0x0b));
	pbuf->Hdr.Code = 0x02;
	pbuf->Hdr.Id = Id;
	pbuf->Hdr.Len2 = pbuf->Hdr.Len1;
	pbuf->Hdr.EapType = 0x14;
	pbuf->Unknown[0] = 0x00;
	pbuf->Unknown[1] = 0x15;
	pbuf->Unknown[2] = 0x04;
	if(m_Data.updateip == 0x01)
		memcpy(pbuf->Ip, m_Data.ip, 4);
	memcpy(pbuf->Username, m_Data.username, strlen(m_Data.username));

	return !::pcap_sendpacket(m_fp, buf, 60);
}



void HuaweiNetwork::GenerateVersion(u_char *buf)
{
	buf[0] = 0x01;
	buf[1] = 0x16;

	unsigned long magic = rand();
	char strMagic[9]={0};
	unsigned char strTemp[4] = {0};
	memcpy(strTemp,(unsigned char *)&magic,4);
	sprintf(strMagic,"%02x%02x%02x%02x",strTemp[0],strTemp[1],strTemp[2],strTemp[3]);
	//printf("%s\n",strMagic);
	unsigned char version[20];
	memset(version,0,sizeof(version));
	memcpy(version,m_ClientVersion,strlen(m_ClientVersion));
	memcpy(version+16,(unsigned char *)&magic,4);

	EncodeVersion(strMagic,version,0x10);
	EncodeVersion("HuaWei3COM1X",version,0x14);
	memcpy(buf+2, version, 20);

	//debug 2
	//for(int i=0; i<20; i++)
	//		  {printf("%02x ",(unsigned char)version[i]);}
	buf[22]=0x02;
	buf[23]=0x16;

	char winVersion[20]={0x3a, 0x71, 0x38, 0x01, 0x0b, 0x3b, 0x7e, 0x3d,
		0x26, 0x7c, 0x7c, 0x17, 0x0b, 0x46, 0x08, 0x32,
		0x32, 0x08, 0x46, 0x0b,
	};
	//unsigned long uWinVersion = GetVersion();
	//sprintf(winVersion,"%u",uWinVersion);
	//EncodeVersion("HuaWei3COM1X", (unsigned char *)winVersion, 0x14);
	memcpy(buf+24, winVersion, 20);
}

//Written By AGanNo2 (AGanNo2@163.com)
void HuaweiNetwork::EncodeVersion(char *strConst,unsigned char *strDest,int iSize)
{

	char *temp=new char[iSize];

	int iTimes = iSize / static_cast<int>(strlen(strConst));

	for(int i=0;i<iTimes;i++)
		memcpy(temp+i*strlen(strConst),strConst,strlen(strConst));

	memcpy(temp+iTimes*strlen(strConst),strConst,iSize%strlen(strConst));

	for(int i=0;i<iSize;i++)
		strDest[i] = strDest[i]^temp[i];

	for(int i=0;i<iSize;i++)
		strDest[iSize-i-1] = strDest[iSize-i-1]^temp[i];

	delete  []temp;
}

