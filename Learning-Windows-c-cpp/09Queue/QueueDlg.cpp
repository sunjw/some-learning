#include <stdlib.h>
#include <string.h>

#include <string>

#include "tchar.h"
#include "process.h"
#include "windows.h"
#include "Windowsx.h"
#include "Strsafe.h"

#include "resource.h"
#include "CommCtrlUI.h"
#include "strhelper.h"
#include "Queue.h"

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

static CQueue s_queue(10);
static volatile LONG s_fShutdow = FALSE;

static HANDLE s_hThreads[MAXIMUM_WAIT_OBJECTS];
static int s_nNumThreads = 0;

DWORD WINAPI ClientThread(PVOID pvParam)
{
	int nThreadNum = PtrToUlong(pvParam);
	HWND hClientListBox = GetDlgItem(s_hDlg, IDC_LIST_CLIENT);

	int nRequestNum = 0;
	while((PVOID)1 != 
		InterlockedCompareExchangePointer((PVOID *)&s_fShutdow, (PVOID)0, (PVOID)0))
	{
		nRequestNum++;

		TCHAR sz[1024];
		CQueue::ELEMENT e = { nThreadNum, nRequestNum };

		if(s_queue.Append(&e, 200))
		{
			StringCchPrintf(sz, _countof(sz), TEXT("Sending %d:%d"),
				nThreadNum, nRequestNum);
		}
		else
		{
			// append error
			StringCchPrintf(sz, _countof(sz), TEXT("Sending %d:%d (%s)"),
				nThreadNum, nRequestNum,
				(GetLastError() == ERROR_TIMEOUT ? TEXT("timeout") : TEXT("full")));
		}

		ListBox_SetCurSel(hClientListBox, ListBox_AddString(hClientListBox, sz));

		Sleep(2500);
	}

	return 0;
}

DWORD WINAPI ServerThread(PVOID pvParam)
{
	int nThreadNum = PtrToUlong(pvParam);
	HWND hServerListBox = GetDlgItem(s_hDlg, IDC_LIST_SERVER);

	while((PVOID)1 != 
		InterlockedCompareExchangePointer((PVOID *)&s_fShutdow, (PVOID)0, (PVOID)0))
	{
		TCHAR sz[1024];
		CQueue::ELEMENT e;

		if(s_queue.Remove(&e, 5000))
		{
			StringCchPrintf(sz, _countof(sz), TEXT("%d: Processing %d:%d"),
				nThreadNum, e.m_nThreadNum, e.m_nRequestNum);

			Sleep(500 * e.m_nThreadNum);
		}
		else
		{
			StringCchPrintf(sz, _countof(sz), TEXT("%d: (timeout)"), nThreadNum);
		}

		ListBox_SetCurSel(hServerListBox, ListBox_AddString(hServerListBox, sz));
	}

	return 0;
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
	DWORD dwThreadID;

	for(int x = 0; x < 4; ++x)
	{
		s_hThreads[s_nNumThreads++] = BEGINTHREADEX(NULL, 
													0, 
													ClientThread, 
													(PVOID)(INT_PTR)x,
													0,
													&dwThreadID);
	}

	for(int x = 0; x < 2; ++x)
	{
		s_hThreads[s_nNumThreads++] = BEGINTHREADEX(NULL, 
													0, 
													ServerThread, 
													(PVOID)(INT_PTR)x,
													0,
													&dwThreadID);
	}
}

static void OnCloseDialog(HWND hDlg, LPARAM lParam)
{
	
}

BOOL CALLBACK DlgMainProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

int WINAPI _tWinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine,
                   int nCmdShow)
{
	s_hInst = hInstance;

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
	InterlockedExchange(&s_fShutdow, TRUE);

	WaitForMultipleObjects(s_nNumThreads, s_hThreads, TRUE, INFINITE);
	while(s_nNumThreads--)
	{
		CloseHandle(s_hThreads[s_nNumThreads]);
	}

	return 0;
}
