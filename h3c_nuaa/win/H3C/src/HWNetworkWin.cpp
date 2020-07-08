#include "stdafx.h"

#include <iphlpapi.h>

#include "HWNetworkWin.h"
#include "MD5_CTX.h"

using namespace std;

HuaweiNetworkWin::HuaweiNetworkWin(HWND hWnd,
							 const string& username,
							 const string& passwd,
							 const string& interfacename, 
							 int* ip, 
							 int* mac,
							 const string& version)
							 :HuaweiNetwork(username,
								passwd,
								interfacename,
								ip,
								mac,
								version),
								hWnd(hWnd)
{}


void HuaweiNetworkWin::SetMd5Buf(PPASSWORDFRM pBuf, const u_char ID, const u_char *chap)
{
	u_char TmpBuf[1 + 64 + 16];
	MD5_CTX md5T;
	u_char digest[16];
	size_t PasswdLen = strlen(m_Data.password);
	TmpBuf[0] = ID;//	memcpy(TmpBuf + 0x00, ID, 1);
	memcpy(TmpBuf + 0x01, m_Data.password, PasswdLen);
	memcpy(TmpBuf + 0x01 + PasswdLen, chap, 16);
	md5T.MD5Update(TmpBuf, static_cast<unsigned int>(17 + PasswdLen));
	md5T.MD5Final(digest);
	memcpy(pBuf->Md5Pwd, digest, 16);
}

void HuaweiNetworkWin::IPRenew(bool bRelease)
{
		// Renew IP Address
	ULONG ulOutBufLen = 0;
    DWORD dwRetVal = 0;
    PIP_INTERFACE_INFO pInfo;
    pInfo = (IP_INTERFACE_INFO *) malloc( sizeof(IP_INTERFACE_INFO));

    // Make an initial call to GetInterfaceInfo to get
    // the necessary size into the ulOutBufLen variable
    if (GetInterfaceInfo(pInfo, &ulOutBufLen) == ERROR_INSUFFICIENT_BUFFER) 
	{
        free( pInfo);
        pInfo = (IP_INTERFACE_INFO *) malloc (ulOutBufLen);
    }

    // Make a second call to GetInterfaceInfo to get the
    // actual data we want
    if ((dwRetVal = GetInterfaceInfo(pInfo, &ulOutBufLen)) == NO_ERROR ) 
	{
        //printf("\tAdapter Name: %ws\n", pInfo->Adapter[0].Name);
        //printf("\tAdapter Index: %ld\n", pInfo->Adapter[0].Index);
        //printf("\tNum Adapters: %ld\n", pInfo->NumAdapters);
    }
    else 
	{

        return;
    }

	int i;
	for (i = 0; i < (int) pInfo->NumAdapters; i++)
	{
		CString temp1((LPCWSTR)(pInfo->Adapter[i].Name + 14));
		CString temp2(m_Data.nic + 12);
		if(!temp1.Compare(temp2))
			break;
	}

	// only my computer currently. 
	// xp: 0 wire, 1 wireless
	// vista: 0 wire, 3 wireless
	if(bRelease)
		dwRetVal = IpReleaseAddress(&pInfo->Adapter[i]); 
	else
		dwRetVal = IpRenewAddress(&pInfo->Adapter[i]);

// Call IpReleaseAddress and IpRenewAddress to release and renew
// the IP address on the first network adapter returned 
// by the call to GetInterfaceInfo.
    
	free(pInfo);
}

void HuaweiNetworkWin::Message(unsigned int msg, const char* str)
{
	::PostMessage(hWnd, msg, NULL, (LPARAM)str);
}

void HuaweiNetworkWin::Message(const char* str)
{
	::PostMessage(hWnd, WM_INFO, NULL, (LPARAM)str);
}

