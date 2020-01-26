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

#define WM_APP_COMPLETED	(WM_APP + 222)

using namespace std;
using namespace sunjwbase;

static HINSTANCE s_hInst = NULL; // Application instance handle
static HWND s_hDlg = NULL; // Dialog handle

static PTP_WORK s_pWorkItem = NULL;
static volatile LONG s_nCurrentTask = 0;

static void AppendMessage(LPCTSTR szMsg)
{
	HWND hListBox = GetDlgItem(s_hDlg, IDC_LIST_MAIN);
	ListBox_SetCurSel(hListBox, ListBox_AddString(hListBox, szMsg));
}

static VOID CALLBACK TaskHandler(PTP_CALLBACK_INSTANCE Instance, 
								 PVOID Context,
								 PTP_WORK Work)
{
	LONG currentTask = InterlockedIncrement(&s_nCurrentTask);
	TCHAR szMsg[MAX_PATH];

	StringCchPrintf(szMsg,
					_countof(szMsg),
					TEXT("[%u] Task #%u is starting."),
					GetCurrentThreadId(),
					currentTask);
	AppendMessage(szMsg);

	Sleep(currentTask * 1000);

	StringCchPrintf(szMsg,
					_countof(szMsg),
					TEXT("[%u] Task #%u is done."),
					GetCurrentThreadId(),
					currentTask);
	AppendMessage(szMsg);

	if(InterlockedDecrement(&s_nCurrentTask) == 0)
	{
		// Notify UI
		PostMessage(s_hDlg, WM_APP_COMPLETED, 0, (LPARAM)currentTask);
	}
}

static void OnStartBatch()
{
	Button_Enable(GetDlgItem(s_hDlg, IDC_BUTTON_START), FALSE);

	AppendMessage(TEXT("----Start!----"));

	SubmitThreadpoolWork(s_pWorkItem);
	SubmitThreadpoolWork(s_pWorkItem);
	SubmitThreadpoolWork(s_pWorkItem);
	SubmitThreadpoolWork(s_pWorkItem);

	AppendMessage(TEXT("4 tasks are submitted"));
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

		case IDC_BUTTON_START:
			OnStartBatch();

			return TRUE;
		} // END switch(LOWORD(wParam))
		break;

	// other message
	case WM_APP_COMPLETED:
		{
			TCHAR szMsg[MAX_PATH];
			StringCchPrintf(szMsg,
					_countof(szMsg),
					TEXT("----Task #%u was the last task----"),
					lParam);
			AppendMessage(szMsg);

			Button_Enable(GetDlgItem(s_hDlg, IDC_BUTTON_START), TRUE);
		}
		break;
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
	s_pWorkItem = CreateThreadpoolWork(TaskHandler, NULL, NULL);
	if(s_pWorkItem == NULL)
	{
		MessageBox(NULL, 
					TEXT("Failed to create the work item for tasks."),
					TEXT(""), 
					MB_ICONSTOP);

		return -1;
	}

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
