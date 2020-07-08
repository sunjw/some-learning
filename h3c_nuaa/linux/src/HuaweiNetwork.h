// HuaweiNetwork.h: interface for the HuaweiNetwork class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HUAWEINETWORK_H__0612B582_B2C6_4682_AAB5_0004AB887CFA__INCLUDED_)
#define AFX_HUAWEINETWORK_H__0612B582_B2C6_4682_AAB5_0004AB887CFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include <iostream>

#include <pcap.h>

#include "global.h"

using namespace std;

#define WM_INFO			1
#define WM_INTERRUPTED	2

extern "C"
{
int pcap_sendpacket(pcap_t *, const u_char *, int);
}

class HuaweiNetwork
{
public:
	HuaweiNetwork(const string& username, const string& passwd,
			const string& interfacenum, int* ip, int* mac,
			const string& version = "2.40-0145");
	virtual ~HuaweiNetwork();

	/*$PROTECTED_FUNCTIONS$*/

	void Connect();
	void DisConnect();

protected:
	char m_ClientVersion[14];
	u_char m_DestMac[6];
	pcap_t *m_fp;
	USERDATA m_Data;
	bool isonline;

	void Connected();
	void ConnectionInterrupted();

	bool OpenAdapter();
	void CloseAdapter();

	void InitBuf(u_char *buf);

	bool SendHello();
	bool SendVersion(const u_char Id);
	virtual bool SendUsername(const u_char Id);
	bool SendPassword(const u_char Id, const u_char *Chap);
	bool SendKeeponline(const u_char Id);
	bool SendLogout();

	/*
	 * Virtual functions
	 * Should be implemented in different os
	 */
	virtual void SetMd5Buf(PPASSWORDFRM pBuf, const u_char ID,
			const u_char *chap) = 0;
	virtual void IPRenew(bool bRelease) = 0;
	virtual void Message(unsigned int msg, const char* str) = 0;
	virtual void Message(const char* str) = 0;

	void MainLoop();

	void GenerateVersion(u_char *buf);
	void EncodeVersion(char *strConst, unsigned char *strDest, int iSize);
};

#endif // !defined(AFX_HUAWEINETWORK_H__0612B582_B2C6_4682_AAB5_0004AB887CFA__INCLUDED_)
