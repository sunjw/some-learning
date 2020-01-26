#include <stdlib.h>
#include <string.h>

#include <string>

#include "tchar.h"
#include "process.h"
#include "windows.h"
#include "Windowsx.h"
#include "Strsafe.h"
#include "Delayimp.h"

#include "resource.h"
#include "CommCtrlUI.h"
#include "strhelper.h"

#include "..\20DelayLoadLib\DelayLoadLib.h"

#pragma comment(lib, "Delayimp.lib")

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

using namespace std;
using namespace sunjwbase;

static HINSTANCE s_hInst = NULL; // Application instance handle
static HWND s_hDlg = NULL; // Dialog handle

static TCHAR g_szDelayLoadModuleName[] = TEXT("20DelayLoadLib");

void ErrMsgBox(PCTSTR pszText)
{
	MessageBox(s_hDlg, 
				pszText,
				TEXT("Error"),
				MB_OK | MB_ICONERROR);
}

void IsModuleLoaded(PCTSTR pszModuleName)
{
	HMODULE hmod = GetModuleHandle(pszModuleName);
	TCHAR szMsg[100];
	StringCchPrintf(szMsg, _countof(szMsg), TEXT("Module \"%s\" is %sloaded"),
		pszModuleName, (hmod == NULL) ? TEXT("not ") : TEXT(""));

	ErrMsgBox(szMsg);
}

static void OnInitDialog(HWND hDlg, LPARAM lParam)
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
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_REVERSE), FALSE);
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

		} // END switch(LOWORD(wParam))
		break;

	// other message

	}

	return FALSE;
}

LONG WINAPI DelayLoadDllExceptionFilter(PEXCEPTION_POINTERS pep)
{
	return 0;
}

int WINAPI _tWinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine,
                   int nCmdShow)
{
	s_hInst = hInstance;

	// other start process
	__try
	{
		int x = 0;

		IsModuleLoaded(g_szDelayLoadModuleName);

		x = fnLib();

		IsModuleLoaded(g_szDelayLoadModuleName);

		x = fnLib2();

		__FUnloadDelayLoadedDLL2("20DelayLoadLib.dll");

		IsModuleLoaded(g_szDelayLoadModuleName);

		x = fnLib();

		IsModuleLoaded(g_szDelayLoadModuleName);
	}
	__except(DelayLoadDllExceptionFilter(GetExceptionInformation()))
	{ }

	//HWND hDlg;
	//hDlg = CreateDialogParam(hInstance, 
	//	MAKEINTRESOURCE(IDD_DIALOG_MAIN), 
	//	NULL, 
	//	DlgMainProc, 
	//	NULL);
	//ShowWindow(hDlg, nCmdShow);

	//// Message loop
	//MSG msg;
	//BOOL ret;
	//while((ret = GetMessage(&msg, 0, 0, 0)) != FALSE) 
	//{
	//	if(ret == -1)
	//		return -1;

	//	if(!IsDialogMessage(hDlg, &msg)) 
	//	{
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}
	//}

	// Other end process
	

	return 0;
}
