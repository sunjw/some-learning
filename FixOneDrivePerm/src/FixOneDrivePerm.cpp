#include <stdio.h>
#include <string>
#include <windows.h>
#include <strsafe.h>
#include <aclapi.h>
#include "strhelper.h"
#include "version.h"

using namespace std;
using namespace sunjwbase;

#define EXE_NAME "FixOneDrivePerm"

class FileLog
{
public:
	FileLog() : m_hLogFile(INVALID_HANDLE_VALUE)
	{
	}
	~FileLog()
	{
		CloseLog();
	}

	void InitLog(const tstring& tstrLogFileName);
	void CloseLog();

	void AppendLog(const string& strLog);
	void AppendLog(const wstring& wstrLog);

private:
	HANDLE m_hLogFile;
};

static FileLog s_fileLog;

static tstring GetCurrentExeDir()
{
	TCHAR tszExeDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, tszExeDir, MAX_PATH);
	for (int i = lstrlen(tszExeDir) - 1; i >= 0; i--)
	{
		if (tszExeDir[i] == TEXT('\\'))
		{
			tszExeDir[i + 1] = 0;
			break;
		}
	}

	return tstring(tszExeDir);
}

void FileLog::InitLog(const tstring& tstrLogFileName)
{
	tstring tstrExeDir = GetCurrentExeDir();
	tstring tstrLogFilePath = tstrExeDir + tstrLogFileName;
	m_hLogFile = CreateFile(tstrLogFilePath.c_str(),
		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hLogFile != INVALID_HANDLE_VALUE)
	{
		// Get file size
		LARGE_INTEGER liFileSize = { 0 };
		GetFileSizeEx(m_hLogFile, &liFileSize);
		LONGLONG llFileSize = liFileSize.QuadPart;
		if (llFileSize > (5 * 1024 * 1024))
		{
			// If log file size is larger than 5MB, clear it.
			SetFilePointer(m_hLogFile, 0, NULL, FILE_BEGIN);
			SetEndOfFile(m_hLogFile);
		}
	}
}

void FileLog::CloseLog()
{
	if (m_hLogFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hLogFile);
		m_hLogFile = INVALID_HANDLE_VALUE;
	}
}

void FileLog::AppendLog(const string& strLog)
{
	if (m_hLogFile == INVALID_HANDLE_VALUE)
		return;

	SetFilePointer(m_hLogFile, 0, NULL, FILE_END);

	string strLogUtf8 = utf8conv((std::string&)strLog);
	DWORD dwBytesWritten = 0;
	WriteFile(m_hLogFile, strLogUtf8.c_str(), strLogUtf8.length(), &dwBytesWritten, NULL);
}

void FileLog::AppendLog(const wstring& wstrLog)
{
	if (m_hLogFile == INVALID_HANDLE_VALUE)
		return;

	SetFilePointer(m_hLogFile, 0, NULL, FILE_END);

	string strLogUtf8 = wstrtostrutf8(wstrLog);
	DWORD dwBytesWritten = 0;
	WriteFile(m_hLogFile, strLogUtf8.c_str(), strLogUtf8.length(), &dwBytesWritten, NULL);
}

static void PrintLog(const string& strLog)
{
	SYSTEMTIME stUTC, stLocal;
	GetSystemTime(&stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

	TCHAR tszTmBuf[1024] = { 0 };
	StringCchPrintf(tszTmBuf, 1024,
		TEXT("%d-%02d-%02d %02d:%02d:%02d.%03d"),
		stLocal.wYear, stLocal.wMonth, stLocal.wDay,
		stLocal.wHour, stLocal.wMinute, stLocal.wSecond,
		stLocal.wMilliseconds);

	string strTime = tstrtostr(tszTmBuf);
	string strFullLog = strTime + " " + strLog;

	printf(strFullLog.c_str());
	s_fileLog.AppendLog(strFullLog);
}

static void PrintLog(const tstring& tstrLog)
{
	SYSTEMTIME stUTC, stLocal;
	GetSystemTime(&stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

	TCHAR tszTmBuf[1024] = { 0 };
	StringCchPrintf(tszTmBuf, 1024,
		TEXT("%d-%02d-%02d %02d:%02d:%02d.%03d"),
		stLocal.wYear, stLocal.wMonth, stLocal.wDay,
		stLocal.wHour, stLocal.wMinute, stLocal.wSecond,
		stLocal.wMilliseconds);

	tstring tstrTime = tszTmBuf;
	tstring tstrFullLog = tstrTime + TEXT(" ") + tstrLog;

#if defined(_WIN32) && (defined(UNICODE) || defined(_UNICODE))
	wprintf(tstrFullLog.c_str());
#else
	printf(tstrFullLog.c_str());
#endif
	s_fileLog.AppendLog(tstrFullLog);
}

static BOOL GetErrorMessage(DWORD dwError, tstring& tstrErrMessage)
{
	LPVOID lpMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	if (lpMsgBuf != NULL)
	{
		tstrErrMessage = (LPCTSTR)lpMsgBuf;
		tstrErrMessage = strtrim(tstrErrMessage);
		LocalFree(lpMsgBuf);
		return TRUE;
	}

	return FALSE;
}

static BOOL IsDirectory(DWORD dwFileAttributes)
{
	return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

static BOOL ModifyPrivilege(LPCTSTR szPrivilege, BOOL fEnable)
{
	BOOL bRet = TRUE;
	TOKEN_PRIVILEGES NewState;
	LUID luid;
	HANDLE hToken = NULL;

	// Open the process token for this process.
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken))
	{
		return FALSE;
	}

	// Get the local unique ID for the privilege.
	if (!LookupPrivilegeValue(NULL,
		szPrivilege,
		&luid))
	{
		CloseHandle(hToken);
		return FALSE;
	}

	// Assign values to the TOKEN_PRIVILEGE structure.
	NewState.PrivilegeCount = 1;
	NewState.Privileges[0].Luid = luid;
	NewState.Privileges[0].Attributes =
		(fEnable ? SE_PRIVILEGE_ENABLED : 0);

	// Adjust the token privilege.
	if (!AdjustTokenPrivileges(hToken,
		FALSE,
		&NewState,
		0,
		NULL,
		NULL))
	{
		bRet = FALSE;
	}

	// Close the handle.
	CloseHandle(hToken);

	return bRet;
}

static BOOL ModifyPrivilegeWithLog(LPCTSTR szPrivilege, BOOL fEnable)
{
	if (!ModifyPrivilege(szPrivilege, fEnable))
	{
		DWORD dwLastErr = GetLastError();
		tstring tstrLog, tstrLastErrMsg;
		GetErrorMessage(dwLastErr, tstrLastErrMsg);
		strappendformat(tstrLog, TEXT("Cannot set privilege [%s], error: 0x%0x, %s\r\n"),
			szPrivilege, dwLastErr, tstrLastErrMsg.c_str());
		PrintLog(tstrLog);
		return FALSE;
	}
	return TRUE;
}

static BOOL PrivilegedProcess()
{
	if (!ModifyPrivilegeWithLog(SE_TAKE_OWNERSHIP_NAME, TRUE))
		return FALSE;

	if (!ModifyPrivilegeWithLog(SE_RESTORE_NAME, TRUE))
		return FALSE;

	if (!ModifyPrivilegeWithLog(SE_BACKUP_NAME, TRUE))
		return FALSE;

	if (!ModifyPrivilegeWithLog(SE_SECURITY_NAME, TRUE))
		return FALSE;

	return TRUE;
}

static DWORD SetPermissions(LPCTSTR path, LPCTSTR userName)
{
	PACL pOldDACL = NULL, pNewDACL = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	EXPLICIT_ACCESS ea;
	DWORD dwRes = ERROR_SUCCESS;
	tstring tstrLog;

	// Get a pointer to the existing DACL.
	dwRes = GetNamedSecurityInfo(path, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
		NULL, NULL, &pOldDACL, NULL, &pSD);
	if (ERROR_SUCCESS != dwRes)
	{
		tstrLog.clear();
		strappendformat(tstrLog, TEXT("GetNamedSecurityInfo Error %u\r\n"), dwRes);
		PrintLog(tstrLog);
		goto Cleanup;
	}

	// Initialize an EXPLICIT_ACCESS structure for the new ACE.
	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
	ea.grfAccessPermissions = GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | DELETE;
	ea.grfAccessMode = SET_ACCESS;
	ea.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
	ea.Trustee.TrusteeType = TRUSTEE_IS_USER;
	ea.Trustee.ptstrName = (LPTSTR)userName;

	// Create a new ACL that merges the new ACE into the existing DACL.
	dwRes = SetEntriesInAcl(1, &ea, pOldDACL, &pNewDACL);
	if (ERROR_SUCCESS != dwRes)
	{
		tstrLog.clear();
		strappendformat(tstrLog, TEXT("SetEntriesInAcl Error %u\r\n"), dwRes);
		PrintLog(tstrLog);
		goto Cleanup;
	}

	// Attach the new ACL as the object's DACL.
	dwRes = SetNamedSecurityInfo((LPTSTR)path, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
		NULL, NULL, pNewDACL, NULL);
	if (ERROR_SUCCESS != dwRes)
	{
		tstrLog.clear();
		strappendformat(tstrLog, TEXT("SetNamedSecurityInfo Error %u\r\n"), dwRes);
		PrintLog(tstrLog);
		goto Cleanup;
	}

Cleanup:
	if (pSD != NULL)
		LocalFree((HLOCAL)pSD);
	if (pNewDACL != NULL)
		LocalFree((HLOCAL)pNewDACL);
	return dwRes;
}

static BOOL TraverseDirectory(const tstring& tstrDirectoryPath, const tstring& tstrUserName)
{
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	tstring tstrLog;

	tstrLog.clear();
	strappendformat(tstrLog, TEXT("Traverse dir:\t[%s]\r\n"), tstrDirectoryPath.c_str());
	PrintLog(tstrLog);

	// Prepare the search string.
	tstring tstrFullPath = tstrDirectoryPath + tstring(TEXT("\\*"));
	hFind = FindFirstFile(tstrFullPath.c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		tstrLog.clear();
		strappendformat(tstrLog, TEXT("FindFirstFile failed (%d)\r\n"), GetLastError());
		PrintLog(tstrLog);
		return FALSE;
	}

	do
	{
		tstring tstrFileName = findFileData.cFileName;
		if (tstrFileName != TEXT(".") && tstrFileName != TEXT(".."))
		{
			tstrFullPath = tstrDirectoryPath + tstring(TEXT("\\")) + tstrFileName;

			BOOL bDirectory = IsDirectory(findFileData.dwFileAttributes);

			if (!bDirectory)
			{
				tstrLog.clear();
				strappendformat(tstrLog, TEXT("Found file:\t[%s]\r\n"), tstrFullPath.c_str());
				PrintLog(tstrLog);
			}

			// Set permissions for the file or directory.
			tstrLog.clear();
			if (ERROR_SUCCESS == SetPermissions(tstrFullPath.c_str(), tstrUserName.c_str()))
				strappendformat(tstrLog, TEXT("SetPermissions:\t[%s] successful.\r\n"), tstrFullPath.c_str());
			else
				strappendformat(tstrLog, TEXT("SetPermissions:\t[%s] failed.\r\n"), tstrFullPath.c_str());
			PrintLog(tstrLog);

			if (bDirectory)
			{
				// Recursively traverse the subdirectory.
				TraverseDirectory(tstrFullPath, tstrUserName);
			}
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);

	return TRUE;
}

static void InitLog()
{
	const tstring tstrLogFileName = TEXT(EXE_NAME ".log");
	s_fileLog.InitLog(tstrLogFileName);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	InitLog();

	PrintLog(TEXT(EXE_NAME " " STR_VERSION " starts.\r\n"));

	int argc;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (argc != 3)
	{
		PrintLog(TEXT("Usage: " EXE_NAME ".exe <machine\\username> <directory>\r\n"));
		return 0;
	}

	tstring tstrUserName = argv[1];
	tstring tstrDirectory = argv[2];

	if (PrivilegedProcess())
	{
		PrintLog(TEXT("Got privileged process.\r\n"));
	}
	else
	{
		PrintLog(TEXT("Failed to make privileged process.\r\n"));
		return 0;
	}

	tstring tstrLog;
	strappendformat(tstrLog, TEXT(EXE_NAME ": <%s> [%s]\r\n"), tstrUserName.c_str(), tstrDirectory.c_str());
	PrintLog(tstrLog.c_str());

	if (TraverseDirectory(tstrDirectory, tstrUserName))
		PrintLog(TEXT("Successfully traversed directory.\r\n"));
	else
		PrintLog(TEXT("Failed to traverse directory.\r\n"));

	return 0;
}
