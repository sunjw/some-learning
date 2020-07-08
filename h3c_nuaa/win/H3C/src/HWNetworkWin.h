#ifndef _HW_NETWORK_WIN_H_
#define _HW_NETWORK_WIN_H_
#include "h3c.h"

#include <string>

#include "HuaweiNetwork.h"

using namespace std;

class HuaweiNetworkWin: public HuaweiNetwork
{
public:
	HuaweiNetworkWin(HWND hWnd, 
		const string& username,
		const string& passwd,
		const string& interfacenum, 
		int* ip, 
		int* mac,
		const string& version = "2.40-0145");

private:
	void SetMd5Buf(PPASSWORDFRM pBuf, const u_char ID, const u_char *chap);
	void IPRenew(bool bRelease);
	void Message(unsigned int msg, const char* str);
	void Message(const char* str);

	HWND hWnd;
};

#endif
