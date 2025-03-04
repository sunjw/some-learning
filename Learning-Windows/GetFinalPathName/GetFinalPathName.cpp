﻿#include <stdio.h>
#include <windows.h>
#include <tchar.h>

#define BUFSIZE MAX_PATH

int _tmain(int argc, TCHAR* argv[])
{
    TCHAR Path[BUFSIZE];
    HANDLE hFile;
    DWORD dwRet;

    printf("\n");
    if (argc != 2)
    {
        wprintf(L"ERROR:\tIncorrect number of arguments\n\n");
        wprintf(L"%s <file_name>\n", argv[0]);
        return 0;
    }

    hFile = CreateFile(argv[1],               // file to open
        GENERIC_READ,          // open for reading
        FILE_SHARE_READ,       // share for reading
        NULL,                  // default security
        OPEN_EXISTING,         // existing file only
        FILE_ATTRIBUTE_NORMAL, // normal file
        NULL);                 // no attr. template

    if (hFile == INVALID_HANDLE_VALUE)
    {
        DWORD dwError = GetLastError();
        LPVOID lpMsgBuf = NULL;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dwError,
            0, //MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), // MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH),
            (LPTSTR)&lpMsgBuf,
            0, NULL);

        wprintf(L"Could not open file: error %d, %s\n", dwError, (LPCTSTR)lpMsgBuf);

        if (lpMsgBuf != NULL)
            LocalFree(lpMsgBuf);

        return 0;
    }

    dwRet = GetFinalPathNameByHandle(hFile, Path, BUFSIZE, VOLUME_NAME_NT);
    if (dwRet < BUFSIZE)
    {
        wprintf(L"The final path (VOLUME_NAME_NT) is: %s\n", Path);
    }
    else
    {
        wprintf(L"The required buffer size is %d.\n", dwRet);
    }
    wprintf(L"\n");

    dwRet = GetFinalPathNameByHandle(hFile, Path, BUFSIZE, VOLUME_NAME_GUID);
    if (dwRet < BUFSIZE)
    {
        wprintf(L"The final path (VOLUME_NAME_GUID) is: %s\n", Path);
    }
    else
    {
        wprintf(L"The required buffer size is %d.\n", dwRet);
    }
    wprintf(L"\n");

    CloseHandle(hFile);

    return 0;
}
