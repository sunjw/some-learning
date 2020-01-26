#include "windows.h"

#include "Queue.h"

CQueue::CQueue(int nMaxElements)
	:m_hmtxQ(m_h[0]), m_hsemNumElements(m_h[1])
{
	m_pElements = (CQueue::PELEMENT)HeapAlloc(GetProcessHeap(),
												0,
												sizeof(CQueue::ELEMENT) * nMaxElements);
	m_nMaxElements = nMaxElements;
	m_hmtxQ = CreateMutex(NULL, FALSE, NULL);
	m_hsemNumElements = CreateSemaphore(NULL, 0, nMaxElements, NULL);
}

CQueue::~CQueue()
{
	CloseHandle(m_hsemNumElements);
	CloseHandle(m_hmtxQ);
	HeapFree(GetProcessHeap(), 0, m_pElements);
}

BOOL CQueue::Append(CQueue::PELEMENT pElement, DWORD dwTimeout)
{
	BOOL fOk = FALSE;
	DWORD dw = WaitForSingleObject(m_hmtxQ, dwTimeout);

	if(dw == WAIT_OBJECT_0)
	{
		// Let's go
		LONG lPrevCount;
		fOk = ReleaseSemaphore(m_hsemNumElements, 1, &lPrevCount);
		if(fOk)
		{
			m_pElements[lPrevCount] = *pElement;
		}
		else
		{
			// Full
			SetLastError(ERROR_DATABASE_FULL);
		}

		ReleaseMutex(m_hmtxQ);
	}
	else
	{
		// Timeout
		SetLastError(ERROR_TIMEOUT);
	}

	return fOk;
}

BOOL CQueue::Remove(CQueue::PELEMENT pElement, DWORD dwTimeout)
{
	BOOL fOk = (WaitForMultipleObjects(_countof(m_h), m_h, TRUE, dwTimeout) 
				== WAIT_OBJECT_0);

	if(fOk)
	{
		// We have something to do.
		*pElement = m_pElements[0];

		// Remove 1st one.
		MoveMemory(&m_pElements[0], &m_pElements[1],
			sizeof(CQueue::ELEMENT) * (m_nMaxElements - 1));

		ReleaseMutex(m_hmtxQ);
	}
	else
	{
		// Timeout
		SetLastError(ERROR_TIMEOUT);
	}

	return fOk;
}
