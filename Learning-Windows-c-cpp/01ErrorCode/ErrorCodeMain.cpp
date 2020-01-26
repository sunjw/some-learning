#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <windows.h>

#include "resource.h"
#include "CommCtrlUI.h"

BOOL CALLBACK DlgMainProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CLOSE:
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
		case IDC_CHECK_ONTOP:
			{
				HWND topFlag = IsDlgButtonChecked(hDlg, IDC_CHECK_ONTOP) 
					? HWND_TOPMOST : HWND_NOTOPMOST;
				SetWindowPos(hDlg, topFlag, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				return TRUE;
			}
		case IDC_BUTTON_LOOK:
			{
				DWORD dwError = GetDlgItemInt(hDlg, 
					IDC_EDIT_ERRORCODE, NULL, FALSE);

				HLOCAL hlocal = NULL;   // Buffer that gets the error message string

				// Use the default system locale since we look for Windows messages.
				// Note: this MAKELANGID combination has 0 as value
				DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);

				// Get the error code's textual description
				BOOL fOk = FormatMessage(
					FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
					FORMAT_MESSAGE_ALLOCATE_BUFFER, 
					NULL, dwError, systemLocale, 
					(PTSTR) &hlocal, 0, NULL);

				if (!fOk) 
				{
					// Is it a network-related error?
					HMODULE hDll = LoadLibraryEx(TEXT("netmsg.dll"), NULL,
						DONT_RESOLVE_DLL_REFERENCES);

					if (hDll != NULL)
					{
						fOk = FormatMessage(
							FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS |
							FORMAT_MESSAGE_ALLOCATE_BUFFER,
							hDll, dwError, systemLocale,
							(PTSTR) &hlocal, 0, NULL);
						FreeLibrary(hDll);
					}
				}

				if (fOk && (hlocal != NULL)) 
				{
					SetDlgItemText(hDlg, IDC_EDIT_MSG, (PCTSTR) LocalLock(hlocal));
					LocalFree(hlocal);
				} 
				else 
				{
					SetDlgItemText(hDlg, IDC_EDIT_MSG,
						TEXT("No text found for this error number."));
				}
				return TRUE;
			}
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
