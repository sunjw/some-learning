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
//#include "CommCtrlUI.h"
#include "strhelper.h"

#include "..\22DipsLib\DipsLib.h"

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

void ErrMsgBox(PCTSTR pszText)
{
	MessageBox(NULL, 
				pszText,
				TEXT("Error"),
				MB_OK | MB_ICONERROR);
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

		case IDC_BUTTON_SAVE:
		case IDC_BUTTON_RESTORE:
		case IDC_BUTTON_CANCEL:
			EndDialog(hDlg, LOWORD(wParam));
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
	CharUpperBuff(lpCmdLine, 1);
	TCHAR cWhatToDo = lpCmdLine[0];

	if(cWhatToDo != TEXT('S') && cWhatToDo != TEXT('R'))
		cWhatToDo = 0;

	if(cWhatToDo == 0)
	{
		int action = DialogBox(hInstance, 
			MAKEINTRESOURCE(IDD_DIALOG_MAIN),
			NULL,
			DlgMainProc);
		switch(action)
		{
		case IDC_BUTTON_SAVE:
			cWhatToDo = TEXT('S');
			break;

		case IDC_BUTTON_RESTORE:
			cWhatToDo = TEXT('R');
			break;

		}
	}

	if(cWhatToDo == 0)
		return 0;

	HWND hWndLv = GetFirstChild(GetFirstChild(
		FindWindow(TEXT("Progman"), NULL)));

	if(!IsWindow(hWndLv))
		return 0;

	SetDIPSHook(GetWindowThreadProcessId(hWndLv, NULL));

	MSG msg;
	GetMessage(&msg, NULL, 0, 0);

	HWND hWndDips = FindWindow(NULL, TEXT("DIPS"));
	if(!IsWindow(hWndDips))
		return 0;

	BOOL bSave = (cWhatToDo == TEXT('S'));

	SendMessage(hWndDips, WM_APP, (WPARAM)hWndLv, bSave);

	SendMessage(hWndDips, WM_CLOSE, 0, 0);

	SetDIPSHook(0);

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
