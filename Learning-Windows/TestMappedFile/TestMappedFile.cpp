#include <stdio.h>
#include <string>
#include <Windows.h>
#include <strsafe.h>

#include "MMFIODef.h"

using namespace std;

const wstring BASE_FILE_NAME(L"MappedFile.txt");

BOOL CreateBaseFile()
{
    // check if file already exists
    DWORD dwAttrib = GetFileAttributes(BASE_FILE_NAME.c_str());
    if (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        // File already exists
        wprintf(L"Base file already exists.\n");
        return TRUE;
    }
    else
    {
        wprintf(L"Creating base file...\n");
    }

    // create a 1KB file
    HANDLE hFile = CreateFile(BASE_FILE_NAME.c_str(), GENERIC_WRITE,
        0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        wprintf(L"Failed to create base file. Error: %lu\n", GetLastError());
        return FALSE;
    }

    BOOL ret = TRUE;

    DWORD dwFileSize = 1024; // 1KB
    DWORD dwPtrLow = SetFilePointer(hFile, (LONG)(dwFileSize & 0xFFFFFFFF),
        NULL, FILE_BEGIN);
    if (dwPtrLow == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
    {
        wprintf(L"Failed to set file pointer. Error: %lu\n", GetLastError());
        ret = FALSE;
    }
    else
    {
        if (!SetEndOfFile(hFile))
        {
            wprintf(L"Failed to set end of file. Error: %lu\n", GetLastError());
            ret = FALSE;
        }
    }

    CloseHandle(hFile);
    return ret;
}

void ShowBaseFileLastModifiedTime()
{
    HANDLE hFile = CreateFile(BASE_FILE_NAME.c_str(), GENERIC_READ,
        FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        wprintf(L"Failed to open base file. Error: %lu\n", GetLastError());
        return;
    }

    FILETIME ftCreate, ftAccess, ftWrite;
    if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
    {
        // Convert the last-write time to local time.
        SYSTEMTIME stUTC, stLocal;
        FileTimeToSystemTime(&ftWrite, &stUTC);
        SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

        wchar_t wzTmBuf[1024] = { 0 };
        StringCchPrintf(wzTmBuf, 1024,
            TEXT("%d-%02d-%02d %02d:%02d:%02d.%03d"),
            stLocal.wYear, stLocal.wMonth, stLocal.wDay,
            stLocal.wHour, stLocal.wMinute, stLocal.wSecond, stLocal.wMilliseconds);
        wprintf(L"Last modified time: %s\n", wzTmBuf);
    }
    else
    {
        wprintf(L"Failed to get file time. Error: %lu\n", GetLastError());
    }

    CloseHandle(hFile);
}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
    if (!CreateBaseFile())
    {
        return 0;
    }

    ShowBaseFileLastModifiedTime();
    {
        wstring lastErrMsg;
        CWinMMFIO mmfio;
        if (!mmfio.Open(BASE_FILE_NAME, NmsMMF::OPN_READWRITE))
        {
            mmfio.GetMMFLastError(lastErrMsg);
            wprintf(L"Failed to open mapped file. Error: %s\n", lastErrMsg.c_str());
            return 0;
        }
    }

    ShowBaseFileLastModifiedTime();

    return 0;
}

