#include <stdio.h>
#include <string>
#include <windows.h>
#include <strsafe.h>
#include <aclapi.h>
#include "strhelper.h"

using namespace std;
using namespace sunjwbase;

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

	void InitLog();
	void CloseLog();

	void AppendLog(const string& strLog);

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

void FileLog::InitLog()
{
	tstring tstrExeDir = GetCurrentExeDir();
	tstring tstrLogFilePath = tstrExeDir + TEXT("FixOneDrivePerm.log");
	m_hLogFile = CreateFile(tstrLogFilePath.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
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

static void PrintLog(const string& strLog)
{
	SYSTEMTIME stUTC, stLocal;
	GetSystemTime(&stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

	TCHAR tstrTmBuf[1024] = { 0 };
	StringCchPrintf(tstrTmBuf, 1024,
		TEXT("%d-%02d-%02d %02d:%02d:%02d.%03d"),
		stLocal.wYear, stLocal.wMonth, stLocal.wDay,
		stLocal.wHour, stLocal.wMinute, stLocal.wSecond,
		stLocal.wMilliseconds);

	string strTime = tstrtostr(tstrTmBuf);

	printf("%s %s", strTime.c_str(), strLog.c_str());
	s_fileLog.AppendLog(strLog);
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
		string strLog;
		GetErrorMessage(dwLastErr, tstrLastErrMsg);
		strappendformat(strLog, "Cannot set privilege [%s], error: 0x%0x, %s\n",
			szPrivilege, dwLastErr, tstrtostr(tstrLastErrMsg).c_str());
		PrintLog(strLog);
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
	string strLog;

	// Get a pointer to the existing DACL.
	dwRes = GetNamedSecurityInfo(path, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
		NULL, NULL, &pOldDACL, NULL, &pSD);
	if (ERROR_SUCCESS != dwRes)
	{
		strLog.clear();
		strappendformat(strLog, "GetNamedSecurityInfo Error %u\n", dwRes);
		PrintLog(strLog);
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
		strLog.clear();
		strappendformat(strLog, "SetEntriesInAcl Error %u\n", dwRes);
		PrintLog(strLog);
		goto Cleanup;
	}

	// Attach the new ACL as the object's DACL.
	dwRes = SetNamedSecurityInfo((LPTSTR)path, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
		NULL, NULL, pNewDACL, NULL);
	if (ERROR_SUCCESS != dwRes)
	{
		strLog.clear();
		strappendformat(strLog, "SetNamedSecurityInfo Error % u\n", dwRes);
		PrintLog(strLog);
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
	string strLog;

	strLog.clear();
	strappendformat(strLog, "Found dir:\t[%s]\n", tstrtostr(tstrDirectoryPath).c_str());
	PrintLog(strLog);

	// Prepare the search string.
	tstring tstrFullPath = tstrDirectoryPath + tstring(TEXT("\\*"));
	hFind = FindFirstFile(tstrFullPath.c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		strLog.clear();
		strappendformat(strLog, "FindFirstFile failed (%d)\n", GetLastError());
		PrintLog(strLog);
		return FALSE;
	}

	do
	{
		tstring tstrFileName = findFileData.cFileName;
		if (tstrFileName != TEXT(".") && tstrFileName != TEXT(".."))
		{
			tstrFullPath = tstrDirectoryPath + tstring(TEXT("\\")) + tstrFileName;

			if (!IsDirectory(findFileData.dwFileAttributes))
			{
				strLog.clear();
				strappendformat(strLog, "Found file:\t[%s]\n", tstrtostr(tstrFullPath).c_str());
				PrintLog(strLog);
			}

			// Set permissions for the file or directory.
			strLog.clear();
			if (ERROR_SUCCESS == SetPermissions(tstrFullPath.c_str(), tstrUserName.c_str()))
				strappendformat(strLog, "SetPermissions:\t[%s] successful.\n", tstrtostr(tstrFullPath).c_str());
			else
				strappendformat(strLog, "SetPermissions:\t[%s] failed.\n", tstrtostr(tstrFullPath).c_str());
			PrintLog(strLog);

			if (IsDirectory(findFileData.dwFileAttributes))
			{
				// Recursively traverse the subdirectory.
				TraverseDirectory(tstrFullPath, tstrUserName);
			}
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);

	return TRUE;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	s_fileLog.InitLog();

	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (argc != 3)
	{
		PrintLog("Usage: FixOneDrivePerm.exe <machine\\username> <directory>\n");
		return 0;
	}

	tstring tstrUserName = argv[1];
	tstring tstrDirectory = argv[2];

	if (PrivilegedProcess())
	{
		PrintLog("Got privileged process.\n");
	}
	else
	{
		PrintLog("Failed to make privileged process.\n");
		return 0;
	}

	if (TraverseDirectory(tstrDirectory, tstrUserName))
		PrintLog("Successfully traversed directory.\n");
	else
		PrintLog("Failed to traverse directory.\n");

	return 0;
}
