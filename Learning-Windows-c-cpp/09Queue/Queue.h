#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <windows.h>

class CQueue
{
public:
	struct ELEMENT
	{
		int m_nThreadNum;
		int m_nRequestNum;
	};
	typedef ELEMENT *PELEMENT;

	CQueue(int nMaxElements);
	~CQueue();

	BOOL Append(PELEMENT pElement, DWORD dwTimeout);
	BOOL Remove(PELEMENT pElement, DWORD dwTimeout);

private:
	PELEMENT m_pElements;
	int m_nMaxElements;
	HANDLE m_h[2];
	HANDLE &m_hmtxQ;			// Queue Mutex
	HANDLE &m_hsemNumElements;	// Queue Semaphore
};

#endif
