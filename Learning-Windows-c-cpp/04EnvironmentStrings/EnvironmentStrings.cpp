#include <stdlib.h>
#include <string.h>

#include <string>

#include <tchar.h>
#include <windows.h>

#include "resource.h"
#include "CommCtrlUI.h"
#include "strhelper.h"

using namespace std;
using namespace sunjwbase;

static HINSTANCE s_hInst = NULL; // Application instance handle
static HWND s_hDlg = NULL; // Application instance handle

static tstring s_tstrMain;

static tstring DumpEnvStrings()
{
	tstring tstrEnvStrings;

	PTSTR ptEnvBlock = GetEnvironmentStrings();
	PTSTR ptszCur = ptEnvBlock;
	PTSTR ptszCurLine = ptszCur;
	int iCurLinePos = 0;

	tstring tstrKey, tstrValue;
	BOOL bFoundKey = FALSE;

	// \0\0 is the end or \0 at the very beginning.
	while(ptszCur != NULL && *ptszCur != TEXT('\0'))
	{
		// line with = at the beginning should be ignored.
		if(iCurLinePos == 0 && 
			ptszCurLine[iCurLinePos] == TEXT('='))
		{
			while(*ptszCur++ != TEXT('\0'));

			ptszCurLine = ptszCur; // new line
			continue;
		}

		if(ptszCurLine[iCurLinePos] == TEXT('\0'))
		{
			// dump env string
			tstrEnvStrings.append(TEXT("\""));
			tstrEnvStrings.append(tstrKey);
			tstrEnvStrings.append(TEXT("\""));
			tstrEnvStrings.append(TEXT("="));
			tstrEnvStrings.append(TEXT("\""));
			tstrEnvStrings.append(tstrValue);
			tstrEnvStrings.append(TEXT("\""));
			tstrEnvStrings.append(TEXT("\r\n"));

			// clean up
			tstrKey = TEXT("");
			tstrValue = TEXT("");
			bFoundKey = FALSE;

			ptszCur += iCurLinePos;
			iCurLinePos = 0;
			ptszCurLine = ++ptszCur; // new line

			continue;
		}

		TCHAR tchCur = ptszCurLine[iCurLinePos++];
		if(!bFoundKey && 
			tchCur == TEXT('='))
		{
			bFoundKey = TRUE;
		}
		else if(!bFoundKey)
		{
			tstrKey.push_back(tchCur);
		}
		else
		{
			tstrValue.push_back(tchCur);
		}

	}

	FreeEnvironmentStrings(ptEnvBlock);

	return tstrEnvStrings;
}

static void UpdateTextMain()
{
	SetDlgItemText(s_hDlg, IDC_EDIT_MAIN, s_tstrMain.c_str());
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
	tstring tstrEnvStrings = DumpEnvStrings();
	//s_tstrMain.append(TEXT("test\r\n"));
	s_tstrMain.append(tstrEnvStrings);

	UpdateTextMain();
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

	return 0;
}
