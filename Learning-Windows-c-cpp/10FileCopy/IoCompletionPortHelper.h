#ifndef _IO_COMPLETION_PORT_H_
#define _IO_COMPLETION_PORT_H_

#include <windows.h>

class CIOCP 
{
public:
	CIOCP(int nMaxConcurrency = -1) 
	{
		m_hIOCP = NULL;
		if (nMaxConcurrency != -1)
			(void)Create(nMaxConcurrency);
	}

	~CIOCP() 
	{
		if (m_hIOCP != NULL)
			CloseHandle(m_hIOCP);
	}

	BOOL Close() 
	{
		BOOL bResult = CloseHandle(m_hIOCP);
		m_hIOCP = NULL;
		return (bResult);
	}

	BOOL Create(int nMaxConcurrency = 0) 
	{
		m_hIOCP = CreateIoCompletionPort(
			INVALID_HANDLE_VALUE, NULL, 0, nMaxConcurrency);
		return (m_hIOCP != NULL);
	}

	BOOL AssociateDevice(HANDLE hDevice, ULONG_PTR CompKey) 
	{
		BOOL fOk = (CreateIoCompletionPort(hDevice, m_hIOCP, CompKey, 0)
			== m_hIOCP);
		return (fOk);
	}

	BOOL AssociateSocket(SOCKET hSocket, ULONG_PTR CompKey) 
	{
		return (AssociateDevice((HANDLE)hSocket, CompKey));
	}

	BOOL PostStatus(ULONG_PTR CompKey, DWORD dwNumBytes = 0,
		OVERLAPPED* po = NULL) 
	{

		BOOL fOk = PostQueuedCompletionStatus(m_hIOCP, dwNumBytes, CompKey, po);
		return (fOk);
	}

	BOOL GetStatus(ULONG_PTR* pCompKey, PDWORD pdwNumBytes,
		OVERLAPPED** ppo, DWORD dwMilliseconds = INFINITE) 
	{
		return (GetQueuedCompletionStatus(m_hIOCP, pdwNumBytes,
			pCompKey, ppo, dwMilliseconds));
	}

private:
	HANDLE m_hIOCP;
};


#endif
