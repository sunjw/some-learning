/*
 * HWNetworkFedora.cpp
 *
 *  Created on: 2010-10-29
 *      Author: sun
 */
#include <cstdlib>
#include <cstring>

#include <netinet/in.h>
#include <openssl/md5.h>

#include "HWNetworkFedora.h"
#include "NetworkInterface.h"

using namespace std;

HuaweiNetworkFedora::HuaweiNetworkFedora(ostream& out, const string& username,
		const string& passwd, const string& interfacename, int* ip, int* mac,
		bool real_ip, const string& version) :
	HuaweiNetwork(username, passwd, interfacename, ip, mac, version),
			outStream(out), send_real_ip(real_ip)
{
}

void HuaweiNetworkFedora::SetMd5Buf(PPASSWORDFRM pBuf, const u_char ID,
		const u_char *chap)
{
	u_char TmpBuf[1 + 64 + 16];
	MD5_CTX md5T;
	size_t PasswdLen = strlen(m_Data.password);
	TmpBuf[0] = ID;//	memcpy(TmpBuf + 0x00, ID, 1);
	memcpy(TmpBuf + 0x01, m_Data.password, PasswdLen);
	memcpy(TmpBuf + 0x01 + PasswdLen, chap, 16);

	MD5_Init(&md5T);
	MD5_Update(&md5T, TmpBuf, 17 + PasswdLen);
	MD5_Final(pBuf->Md5Pwd, &md5T);
	//memcpy(pBuf->Md5Pwd, md5T.digest, 16);
}

void HuaweiNetworkFedora::IPRenew(bool bRelease)
{
	string id("\"System ");
	id.append(m_Data.nic);
	id.append("\"");

	if (bRelease)
	{
		string cmd("nmcli con down id ");
		cmd.append(id);
		Message(0, "释放 IP");
		Message(0, cmd.c_str());
		system(cmd.c_str());
	}
	else
	{
		string cmd("nmcli con up id ");
		cmd.append(id);
		Message(0, "申请 IP");
		Message(0, cmd.c_str());
		system(cmd.c_str());
	}

}

void HuaweiNetworkFedora::Message(unsigned int msg, const char* str)
{
	outStream << str << endl;
}

void HuaweiNetworkFedora::Message(const char* str)
{
	outStream << str << endl;
}

bool HuaweiNetworkFedora::SendUsername(const u_char Id)
{
	u_char buf[100] =
	{ 0 };
	PUSERNAMEFRM pbuf = (PUSERNAMEFRM) buf;

	if (send_real_ip)
	{
		string ip = GetInterfaceIP(m_Data.nic);
		//Message(ip.c_str());
		if (ip.length() > 0)
		{
			size_t old_pos = 0;
			size_t dot_pos = ip.find('.', old_pos);
			for (int i = 0; i < 4; ++i)
			{
				string part;
				if (dot_pos && i < 3)
				{
					part = ip.substr(old_pos, dot_pos - old_pos);
					m_Data.ip[i] = atoi(part.c_str());
					old_pos = dot_pos + 1;
					dot_pos = ip.find('.', old_pos);
				}
				if (i == 3)
				{
					part = ip.substr(old_pos, ip.length() - old_pos);
					m_Data.ip[i] = atoi(part.c_str());
				}
				//Message(part);
			}

		}
	}

	InitBuf(buf);
	pbuf->Hdr.Len1 = htons(
			static_cast<u_short> (strlen(m_Data.username) + 0x0b));
	pbuf->Hdr.Code = 0x02;
	pbuf->Hdr.Id = Id;
	pbuf->Hdr.Len2 = pbuf->Hdr.Len1;
	pbuf->Hdr.EapType = 0x01;
	pbuf->Unknown[0] = 0x15;
	pbuf->Unknown[1] = 0x04;
	if (m_Data.updateip == 0x01)
		memcpy(pbuf->Ip, m_Data.ip, 4);
	memcpy(&pbuf->Username, &m_Data.username, strlen(m_Data.username));

	if (!::pcap_sendpacket(m_fp, buf, 60))
	{
		//puts("Verifying User Name ...");
		//char *t="Verifying User Name ...";
		////::PostMessage(hWnd,WM_INFO,NULL,(LPARAM)t);
		return true;
	}

	return false;
}
