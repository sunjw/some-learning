// MMFIO.cpp : Defines the entry point for the console application.
//
#include "MMFIODef.h"
#include <tchar.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MMF_ERR_ZERO_BYTE_FILE           _T("Cannot open zero byte file.")
#define MMF_ERR_INVALID_SET_FILE_POINTER _T("The file pointer cannot be set to specified location.")
#define MMF_ERR_WRONG_OPEN               _T("Close previous file before opening another.")
#define MMF_ERR_OPEN_FILE                _T("Error encountered during file open.")
#define MMF_ERR_CREATEFILEMAPPING        _T("Failed to create file mapping object.")
#define MMF_ERR_MAPVIEWOFFILE            _T("Failed to map view of file.")
#define MMF_ERR_SETENDOFFILE             _T("Failed to set end of file.")
#define MMF_ERR_INVALIDSEEK              _T("Seek request lies outside file boundary.")
#define MMF_ERR_WRONGSEEK                _T("Offset must be negative while seeking from file end.");

//~~~ CWinMMFIO implementation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CWinMMFIO::CWinMMFIO()
{
	SYSTEM_INFO sinf;
	GetSystemInfo(&sinf);
	m_dwAllocGranularity = sinf.dwAllocationGranularity;
	m_lExtendOnWriteLength = m_dwAllocGranularity;

	m_dwBytesInView = m_dwAllocGranularity;
	m_nCurPos = 0;
	m_nViewBegin = 0;
	m_pbFile = 0;
	m_hFileMapping = INVALID_HANDLE_VALUE;
	m_cRefCount = 0;
	m_bFileExtended = false;
}

void CWinMMFIO::_close()
{
	_checkFileExtended();

	if(m_pbFile)
	{
		//unmap view 
		FlushViewOfFile(m_pbFile, 0);
		UnmapViewOfFile(m_pbFile);
		m_pbFile = NULL;
	}

	if(m_hFileMapping)
	{
		//close mapping object handle
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
	}

	if(m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}
}

CWinMMFIO::~CWinMMFIO()
{
	_close();
}

bool CWinMMFIO::Open(const sstring& strfile, OPENFLAGS oflags)
{
#ifdef _DEBUG
	SysAssert(m_cRefCount == 0);
#endif

	if(m_cRefCount!=0)
	{
		m_strErrMsg = MMF_ERR_WRONG_OPEN;
		return false;
	}

	m_cRefCount++;

	// Open the data file.
	DWORD dwflags;
	dwflags = (OPN_READWRITE == oflags)?
		GENERIC_READ | GENERIC_WRITE: GENERIC_READ;

	m_hFile = CreateFile(strfile.c_str(), dwflags,
		FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	HANDLE hFile=m_hFile;

	if(INVALID_HANDLE_VALUE == hFile)
	{
		m_strErrMsg = MMF_ERR_OPEN_FILE;
		return false;
	}

	DWORD dwFileSizeHigh;
	m_qwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
	m_qwFileSize += (((sint64) dwFileSizeHigh) << 32);

	if(m_qwFileSize == 0)
	{
		CloseHandle(hFile);
		m_strErrMsg = MMF_ERR_ZERO_BYTE_FILE;
		return false;
	}

	// Create the file-mapping object.
	m_dwflagsFileMapping = (OPN_READWRITE == oflags)?PAGE_READWRITE: PAGE_READONLY;

	m_hFileMapping = CreateFileMapping(hFile, NULL, m_dwflagsFileMapping, 0, 
		0, 0);

	if(NULL == m_hFileMapping)
	{
		if(INVALID_HANDLE_VALUE == hFile)CloseHandle(hFile);
		m_strErrMsg = MMF_ERR_CREATEFILEMAPPING;
		return false;
	}

	if(m_qwFileSize <= m_dwBytesInView)
		m_dwBytesInView = m_qwFileSize;

	//m_dwflagsView = (OPN_WRITE == oflags || OPN_READWRITE == oflags)?
	//FILE_MAP_WRITE: FILE_MAP_READ;
	m_dwflagsView = FILE_MAP_ALL_ACCESS;

	m_pbFile = (PBYTE)MapViewOfFile(m_hFileMapping, m_dwflagsView, 0, 0, 
		m_dwBytesInView);

	if(NULL == m_pbFile)
	{
		CloseHandle(m_hFileMapping);
		m_strErrMsg = MMF_ERR_MAPVIEWOFFILE;
		return false;
	}

	m_nCurPos = 0;
	m_nViewBegin = 0;
	m_bFileExtended = false;

	return true;
}

bool CWinMMFIO::Close()
{
#ifdef _DEBUG
	SysAssert(m_cRefCount == 1);
#endif

	m_cRefCount--;
	_close();
	return true;
}


int CWinMMFIO::Read(void* pBuf, quint nCountIn)
{
	if(nCountIn ==0)return 0;

	_checkFileExtended();

	if(m_nCurPos >= m_qwFileSize)return 0;

	int nCount = nCountIn;//int is used to detect any bug

	m_dwBytesInView = m_dwAllocGranularity;
	//check if m_nViewBegin+m_dwBytesInView crosses filesize
	if(m_nViewBegin + m_dwBytesInView > m_qwFileSize)
	{
		m_dwBytesInView = m_qwFileSize - m_nViewBegin;
	}

	sint64 nDataEndPos = m_nCurPos + nCount;
	if(nDataEndPos >= m_qwFileSize)
	{
		nDataEndPos = m_qwFileSize;
		nCount = m_qwFileSize - m_nCurPos;
	}

	SysAssert(nCount >= 0);//nCount is int, if -ve then error

	sint64 nViewEndPos = m_nViewBegin + m_dwBytesInView;

	if(nDataEndPos < nViewEndPos)
	{
		memcpy_s(pBuf, nCountIn, m_pbFile + (m_nCurPos-m_nViewBegin), nCount);
		m_nCurPos += nCount;
	}
	else if(nDataEndPos == nViewEndPos)
	{
		//Last exact bytes are read from the view
		memcpy_s(pBuf, nCountIn, m_pbFile + (m_nCurPos-m_nViewBegin), nCount);
		m_nCurPos += nCount;

		_seek(m_nCurPos, SP_BEGIN);
		nViewEndPos = m_nViewBegin + m_dwBytesInView;
	}
	else
	{
		LPBYTE pBufRead = (LPBYTE)pBuf;
		if(nDataEndPos > nViewEndPos)
		{
			//nDataEndPos can span multiple view blocks
			while(m_nCurPos < nDataEndPos)
			{
				int nReadBytes = nViewEndPos - m_nCurPos;

				if(nViewEndPos > nDataEndPos)
					nReadBytes = nDataEndPos - m_nCurPos;

				memcpy_s(pBufRead, nCountIn, m_pbFile + (m_nCurPos-m_nViewBegin), nReadBytes);
				pBufRead += nReadBytes;

				m_nCurPos += nReadBytes;
				//seeking does view remapping if m_nCurPos crosses view boundary
				_seek(m_nCurPos, SP_BEGIN);
				nViewEndPos = m_nViewBegin + m_dwBytesInView;
			}
		}
	}

	return nCount;
}

bool CWinMMFIO::SetLength(const sint64& nLength)
{
	//unmap view 
	UnmapViewOfFile(m_pbFile);
	//close mapping object handle
	CloseHandle(m_hFileMapping);

	LONG nLengthHigh = (nLength >> 32);
	DWORD dwPtrLow = SetFilePointer(m_hFile, (LONG) (nLength & 0xFFFFFFFF),
		&nLengthHigh, FILE_BEGIN);

	if(INVALID_SET_FILE_POINTER == dwPtrLow && GetLastError() != NO_ERROR)
	{
		m_strErrMsg = MMF_ERR_INVALID_SET_FILE_POINTER;
		return false;
	}
	//set the eof to the file pointer position
	if(SetEndOfFile(m_hFile) == 0)
	{
		m_strErrMsg = MMF_ERR_SETENDOFFILE;
		return false;
	}

	m_qwFileSize = nLength;

	//call CreateFileMapping 
	m_hFileMapping = CreateFileMapping(m_hFile, NULL, m_dwflagsFileMapping,
		0, 0, _T("SMP"));

	//remap here
	m_pbFile = (PBYTE)MapViewOfFile(m_hFileMapping, m_dwflagsView, 
		(DWORD) (m_nViewBegin >> 32), (DWORD) (m_nViewBegin & 0xFFFFFFFF), 
		m_dwBytesInView);

	return true;
}

int CWinMMFIO::Write(void* pBuf, quint nCount)
{
	if(nCount == 0)return 0;

	sint64 nViewEndPos = m_nViewBegin + m_dwBytesInView;
	sint64 nDataEndPos = m_nCurPos + nCount;

	if(nDataEndPos > nViewEndPos)
	{
		if(nDataEndPos >= m_qwFileSize)
		{
			//Extend the end position by m_lExtendOnWriteLength bytes
			sint64 nNewFileSize = nDataEndPos + m_lExtendOnWriteLength;

			if(SetLength(nNewFileSize))
			{
				m_bFileExtended = true;
			}
		}

		LPBYTE pBufWrite = (LPBYTE)pBuf;
		while(m_nCurPos < nDataEndPos)
		{
			int nWriteBytes = nViewEndPos - m_nCurPos;

			if(nViewEndPos > nDataEndPos)
				nWriteBytes = nDataEndPos - m_nCurPos;

			memcpy_s(&m_pbFile[m_nCurPos-m_nViewBegin], m_dwBytesInView, pBufWrite, nWriteBytes);
			pBufWrite += nWriteBytes;

			m_nCurPos += nWriteBytes;
			//seeking does view remapping if m_nCurPos crosses view boundary
			_seek(m_nCurPos, SP_BEGIN);
			nViewEndPos = m_nViewBegin + m_dwBytesInView;
		}
	}
	else
	{
		//nCount bytes lie within the current view
		memcpy_s(&m_pbFile [m_nCurPos-m_nViewBegin], nCount, pBuf, nCount);
		m_nCurPos += nCount;
	}

	return nCount;
}

void CWinMMFIO::_flush()
{
}

bool CWinMMFIO::Seek(sint64 lOffset/*can be -ve */, SEEKPOS eseekpos)
{
	_checkFileExtended();
	bool bRet = _seek(lOffset, eseekpos);
	return bRet;
}

bool CWinMMFIO::_seek(sint64 lOffset/*can be -ve */, SEEKPOS eseekpos)
{
	if(SP_CUR == eseekpos)
	{
		lOffset = m_nCurPos + lOffset;
	}
	else if(SP_END == eseekpos)
	{
		if(lOffset >= 0)
		{
			m_strErrMsg = MMF_ERR_WRONGSEEK;
			return false;
		}

		//lOffset in RHS is -ve
		lOffset = m_qwFileSize + lOffset;
	}
	//else means SP_BEGIN


	//lOffset must be less than the file size
	if(!(lOffset >= 0 && lOffset < m_qwFileSize))
	{
		m_strErrMsg = MMF_ERR_INVALIDSEEK;
		return false;
	}

	if(!(lOffset >= m_nViewBegin && lOffset < m_nViewBegin + m_dwBytesInView))
	{
		//lOffset lies outside the mapped view, remap the view
		sint64 _N = (sint64)floor((double)lOffset/((double)m_dwAllocGranularity));
		m_nViewBegin = _N*m_dwAllocGranularity;
		m_dwBytesInView = m_dwAllocGranularity;
		//check if m_nViewBegin+m_dwBytesInView crosses filesize
		if(m_nViewBegin + m_dwBytesInView > m_qwFileSize)
		{
			m_dwBytesInView = m_qwFileSize - m_nViewBegin;
		}
		if(m_dwBytesInView != 0 && m_pbFile)
		{
			//Unmap old view
			UnmapViewOfFile(m_pbFile);
			//Remap with new starting address
			m_pbFile = (PBYTE)MapViewOfFile(m_hFileMapping, m_dwflagsView, 
				(DWORD) (m_nViewBegin >> 32), (DWORD) (m_nViewBegin & 0xFFFFFFFF), 
				m_dwBytesInView);

			//DWORD err = GetLastError();
		}
	}

	m_nCurPos = lOffset;
	return true;
}

suint64 CWinMMFIO::GetLength()
{
	_checkFileExtended();
	return m_qwFileSize;
}

suint64 CWinMMFIO::GetPosition()
{
	return m_nCurPos;
}

/*
If file is extended in Write function then this must be called to re-adjust
the file to its actual length before Seek or Read or any such fuction.
*/
bool CWinMMFIO::_checkFileExtended()
{
	bool bRet = true;
	if(m_bFileExtended)
	{
		//remove extra bytes
		bRet = SetLength(m_nCurPos);
	}
	m_bFileExtended = false;
	return bRet;
}
