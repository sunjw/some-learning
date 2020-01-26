#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <string>

#include "tchar.h"
#include "process.h"
#include "windows.h"
#include "Windowsx.h"
#include "Strsafe.h"
#include "TlHelp32.h"

#include "resource.h"
#include "CommCtrlUI.h"
#include "strhelper.h"

typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define BEGINTHREADEX(psa, cbStackSize, pfnStartAddr,	\
   pvParam, dwCreateFlags, pdwThreadId)                 \
      ((HANDLE)_beginthreadex(                          \
         (void *)        (psa),                         \
         (unsigned)      (cbStackSize),                 \
         (PTHREAD_START) (pfnStartAddr),                \
         (void *)        (pvParam),                     \
         (unsigned)      (dwCreateFlags),               \
         (unsigned *)    (pdwThreadId)))

#define FILENAME TEXT("FileRev.dat")

using namespace std;
using namespace sunjwbase;

static HINSTANCE s_hInst = NULL; // Application instance handle
static HWND s_hDlg = NULL; // Dialog handle

void ErrMsgBox(PCTSTR pszCaption, PCTSTR pszText)
{
	MessageBox(s_hDlg, 
				pszText,
				pszCaption,
				MB_OK | MB_ICONERROR);
}

void ErrMsgBox(PCTSTR pszText)
{
	ErrMsgBox(TEXT("Error"), pszText);
}

void InfoMsgBox(PCTSTR pszCaption, PCTSTR pszText)
{
	MessageBox(s_hDlg, 
				pszText,
				pszCaption,
				MB_OK | MB_ICONINFORMATION);
}

void InfoMsgBox(PCTSTR pszText)
{
	InfoMsgBox(TEXT("Info"), pszText);
}

#ifdef UNICODE
#define InjectLib InjectLibW
#define EjectLib EjectLibW
#else
#define InjectLib InjectLibA
#define EjectLib EjectLibA
#endif

BOOL WINAPI InjectLibW(DWORD dwProcessId, PCWSTR pszLibFile)
{
	BOOL bOk = FALSE;

	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	PWSTR pszLibFileRemote = NULL;

	__try
	{
		hProcess = OpenProcess(
			PROCESS_QUERY_INFORMATION |
			PROCESS_CREATE_THREAD |
			PROCESS_VM_OPERATION |
			PROCESS_VM_WRITE,
			FALSE, dwProcessId);

		if(hProcess == NULL)
			__leave;

		int cch = 1 + lstrlenW(pszLibFile);
		int cb = cch * sizeof(wchar_t);

		// Allocate space in remote process.
		pszLibFileRemote = (PWSTR)VirtualAllocEx(
			hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);

		if(pszLibFileRemote == NULL)
			__leave;

		// Copy data into space
		if(!WriteProcessMemory(hProcess, pszLibFileRemote,
			(PVOID)pszLibFile, cb, NULL))
			__leave;

		// Assume that kernel32 is at the same address in remote process.
		PTHREAD_START_ROUTINE pfnThreadStart = (PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
		
		if(pfnThreadStart == NULL)
			__leave;

		// Create remote thread.
		hThread = CreateRemoteThread(hProcess, NULL, 0,
			pfnThreadStart, pszLibFileRemote, 0, NULL);

		if(hThread == NULL)
			__leave;

		WaitForSingleObject(hThread, INFINITE);

		bOk = TRUE;
	}
	__finally
	{
		if(pszLibFileRemote != NULL)
			VirtualFreeEx(hProcess, pszLibFileRemote, 0, MEM_RELEASE);

		if(hThread != NULL)
			CloseHandle(hThread);

		if(hProcess != NULL)
			CloseHandle(hProcess);
	}

	return bOk;
}

BOOL WINAPI InjectLibA(DWORD dwProcessId, PCSTR pszLibFile)
{
	SIZE_T cchSize = lstrlenA(pszLibFile) + 1;
	// _alloca allocate space on stack
	PWSTR pszLibFileW = (PWSTR)_alloca(cchSize * sizeof(wchar_t));

	StringCchPrintfW(pszLibFileW, cchSize, L"%S", pszLibFile);

	return InjectLibW(dwProcessId, pszLibFileW);
}

BOOL WINAPI EjectLibW(DWORD dwProcessId, PCWSTR pszLibFile)
{
	BOOL bOk = FALSE;

	HANDLE hthSnapshot = NULL;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;

	__try
	{
		hthSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
		if(hthSnapshot == INVALID_HANDLE_VALUE)
			__leave;

		// Get our dll module.
		MODULEENTRY32W me = { sizeof(me) };
		BOOL bFound = FALSE;
		BOOL bMoreMods = Module32FirstW(hthSnapshot, &me);
		for(; bMoreMods; bMoreMods = Module32NextW(hthSnapshot, &me))
		{
			bFound = (_wcsicmp(me.szModule, pszLibFile) == 0) ||
				(_wcsicmp(me.szExePath, pszLibFile) == 0);
			
			if(bFound)
				break;
		}

		if(!bFound)
			__leave;

		hProcess = OpenProcess(
			PROCESS_QUERY_INFORMATION |
			PROCESS_CREATE_THREAD |
			PROCESS_VM_OPERATION,
			FALSE, dwProcessId);

		if(hProcess == NULL)
			__leave;

		// Assume that kernel32 is at the same address in remote process.
		PTHREAD_START_ROUTINE pfnThreadStart = (PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "FreeLibrary");
		
		if(pfnThreadStart == NULL)
			__leave;

		// Create remote thread.
		hThread = CreateRemoteThread(hProcess, NULL, 0,
			pfnThreadStart, me.modBaseAddr, 0, NULL);

		if(hThread == NULL)
			__leave;

		WaitForSingleObject(hThread, INFINITE);

		bOk = TRUE;
	}
	__finally
	{
		if(hthSnapshot != NULL)
			CloseHandle(hthSnapshot);

		if(hThread != NULL)
			CloseHandle(hThread);

		if(hProcess != NULL)
			CloseHandle(hProcess);
	}

	return bOk;
}

BOOL WINAPI EjectLibA(DWORD dwProcessId, PCSTR pszLibFile)
{
	SIZE_T cchSize = lstrlenA(pszLibFile) + 1;
	// _alloca allocate space on stack
	PWSTR pszLibFileW = (PWSTR)_alloca(cchSize * sizeof(wchar_t));

	StringCchPrintfW(pszLibFileW, cchSize, L"%S", pszLibFile);

	return EjectLibW(dwProcessId, pszLibFileW);
}

void OnInitDialog(HWND hDlg, LPARAM lParam)
{
	// Save dialog instance
	s_hDlg = hDlg;

	// Load icon
	HICON hIcon;
	hIcon = (HICON)LoadImage(s_hInst,
				MAKEINTRESOURCE(IDI_ICON_WZIP),
                IMAGE_ICON,
                GetSystemMetrics(SM_CXSMICON),
                GetSystemMetrics(SM_CYSMICON),
                0);
	if(hIcon)
	{
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	}

	// Other process

}

static void OnCloseDialog(HWND hDlg, LPARAM lParam)
{
	
}

INT_PTR CALLBACK DlgMainProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{


	switch(message)
	{
	case WM_INITDIALOG:
		OnInitDialog(hDlg, lParam);
		return TRUE;
	case WM_CLOSE:
		OnCloseDialog(hDlg, lParam);

		DestroyWindow(hDlg);
		return TRUE;
	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			SendMessage(hDlg, WM_CLOSE, 0, 0);
			return TRUE;

		// other WM_COMMAND
		case IDC_BUTTON_EXIT:
			SendMessage(hDlg, WM_CLOSE, 0, 0);
			return TRUE;

		case IDC_BUTTON_INJECT:
			DWORD dwProcessId = GetDlgItemInt(hDlg, IDC_EDIT_PID, NULL, FALSE);
			if(dwProcessId == 0)
				dwProcessId = GetCurrentProcessId();

			//TCHAR szLibFile[] = TEXT("c:\\sunjw.doc\\Programs\\cpp\\Windows Programming\\src\\Windows\\x64\\Debug\\20DelayLoadLib.dll");
			TCHAR szCurModule[MAX_PATH] = {0};
			TCHAR szLibPath[MAX_PATH] = {0};
	
			GetModuleFileName(NULL, szCurModule, _countof(szCurModule));

			TCHAR *pFileName = _tcsrchr(szCurModule, TEXT('\\')) + 1;
			*pFileName = 0;
			_tcscpy_s(szLibPath, (long)pFileName - (long)szCurModule, szCurModule);
			wcscat(szLibPath, TEXT("22ImgWalkLib.dll"));

			if(InjectLib(dwProcessId, szLibPath))
			{
				EjectLib(dwProcessId, szLibPath);

				InfoMsgBox(TEXT("DLL Inject/Eject successful."));
			}
			else
			{
				ErrMsgBox(TEXT("DLL Inject/Eject failed."));
			}

			return TRUE;

		} // END switch(LOWORD(wParam))
		break;

	// other message

	}

	return FALSE;
}

int WINAPI _tWinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine,
                   int nCmdShow)
{
	s_hInst = hInstance;

	// other start process

	HWND hDlg;
	hDlg = CreateDialogParam(hInstance, 
		MAKEINTRESOURCE(IDD_DIALOG_MAIN), 
		NULL, 
		DlgMainProc, 
		NULL);
	ShowWindow(hDlg, nCmdShow);

	// Message loop
	MSG msg;
	BOOL ret;
	while((ret = GetMessage(&msg, 0, 0, 0)) != FALSE) 
	{
		if(ret == -1)
			return -1;

		if(!IsDialogMessage(hDlg, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Other end process


	return 0;
}
