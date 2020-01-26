#include <stdlib.h>
#include <string.h>

#include <string>

#include <tchar.h>
#include <windows.h>
#include <Sddl.h>
#include <Strsafe.h>

#include "resource.h"
#include "CommCtrlUI.h"
#include "strhelper.h"

using namespace std;
using namespace sunjwbase;

static HINSTANCE s_hInst = NULL; // Application instance handle

static HANDLE s_hSingleton = NULL;
static HANDLE s_hBoundary = NULL;
static HANDLE s_hNamespace = NULL;

static BOOL s_bNamespaceOpened = FALSE;

static PCTSTR s_tszBoundary = TEXT("03-Boundary");
static PCTSTR s_tsznNamespace = TEXT("03-Namespace");

static string s_strMain;

static void UpdateTextMain(HWND hDlg)
{
	SetDlgItemText(hDlg, IDC_EDIT_MAIN, strtotstr(s_strMain).c_str());
}

static void CheckInstance(HWND hDlg)
{
	// Create boundary descriptor
	s_hBoundary = CreateBoundaryDescriptor(s_tszBoundary, 0);

	// Create a SID related to local administrator group
	BYTE localAdminSID[SECURITY_MAX_SID_SIZE];
	PSID pLocalAdminSID = &localAdminSID;
	DWORD cbSID = sizeof(localAdminSID);
	if(!CreateWellKnownSid(WinBuiltinAdministratorsSid, 
		NULL,
		pLocalAdminSID,
		&cbSID))
	{
		strappendformat(s_strMain, 
			"CreateWellKnownSid failed: %u\r\n",
			GetLastError());
		UpdateTextMain(hDlg);
		return;
	}

	if(!AddSIDToBoundaryDescriptor(&s_hBoundary, pLocalAdminSID))
	{
		strappendformat(s_strMain, 
			"AddSIDToBoundaryDescriptor failed: %u\r\n",
			GetLastError());
		UpdateTextMain(hDlg);
		return;
	}

	// Create the namespace for Local Administrators only
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = FALSE;
	if(!ConvertStringSecurityDescriptorToSecurityDescriptor(
		TEXT("D:(A;;GA;;;BA)"),
		SDDL_REVISION_1,
		&sa.lpSecurityDescriptor,
		NULL))
	{
		strappendformat(s_strMain, 
			"ConvertStringSecurityDescriptorToSecurityDescriptor failed: %u\r\n",
			GetLastError());
		UpdateTextMain(hDlg);
		return;
	}

	s_hNamespace = CreatePrivateNamespace(&sa, s_hBoundary, s_tsznNamespace);

	// Free security descriptor
	LocalFree(sa.lpSecurityDescriptor);

	DWORD dwLastError = GetLastError();
	if(s_hNamespace == NULL)
	{
		// access denied
		if(dwLastError == ERROR_ACCESS_DENIED)
		{
			strappendformat(s_strMain, 
				"Access denied when creating the namespace.\r\n");
			strappendformat(s_strMain, 
				"  You must be running as Administrator.\r\n\r\n");
			UpdateTextMain(hDlg);
			return;
		}
		else
		{
			if(dwLastError == ERROR_ALREADY_EXISTS)
			{
				// Let's open it
				strappendformat(s_strMain, 
					"CreatePrivateNamespace failed: %u\r\n",
					dwLastError);

				s_hNamespace = OpenPrivateNamespace(s_hBoundary, s_tsznNamespace);
				if(s_hNamespace == NULL)
				{
					strappendformat(s_strMain, 
						"  and OpenPrivateNamespace failed: %u\r\n",
						dwLastError);
					UpdateTextMain(hDlg);
					return;
				}
				else
				{
					s_bNamespaceOpened = TRUE;
					strappendformat(s_strMain, 
						"  but OpenPrivateNamespace succeeded\r\n",
						dwLastError);
				}
			}
			else
			{
				strappendformat(s_strMain, 
						"Unexpected error occured: %u\r\n",
						dwLastError);
				UpdateTextMain(hDlg);
				return;
			}
		}
	}

	// Try to create mutex
	TCHAR tszMutexName[64];
	StringCchPrintf(tszMutexName, 
		_countof(tszMutexName),
		TEXT("%s\\%s"),
		s_tsznNamespace,
		TEXT("Singleton"));

	s_hSingleton = CreateMutex(NULL, FALSE, tszMutexName);
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		strappendformat(s_strMain, 
			"Another instance of Singleton is running:\r\n");
		strappendformat(s_strMain, 
			"--> Impossible to access application features.\r\n");
	}
	else
	{
		strappendformat(s_strMain, 
			"First instance of Singleton:\r\n");
		strappendformat(s_strMain, 
			"--> Access application features now.\r\n");
	}

	UpdateTextMain(hDlg);
}

static void OnInitDialog(HWND hDlg, LPARAM lParam)
{
	// Load icon
	HICON hIcon;
	hIcon = (HICON)LoadImage(s_hInst,
				MAKEINTRESOURCE(IDI_ICON_FIFA),
                IMAGE_ICON,
                GetSystemMetrics(SM_CXSMICON),
                GetSystemMetrics(SM_CYSMICON),
                0);
	if(hIcon)
	{
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	}

	// Other process
	CheckInstance(hDlg);

	//strappendformat(s_strMain, "sdfasf\r\n");
	//UpdateTextMain(hDlg);
}

static void OnCloseDialog(HWND hDlg, LPARAM lParam)
{
	if(s_hSingleton != NULL)
		CloseHandle(s_hSingleton);

	if(s_hNamespace != NULL)
	{
		if(s_bNamespaceOpened)
			ClosePrivateNamespace(s_hNamespace, 0); // close opened
		else
			ClosePrivateNamespace(s_hNamespace, PRIVATE_NAMESPACE_FLAG_DESTROY);
	}

	if(s_hBoundary != NULL)
		DeleteBoundaryDescriptor(s_hBoundary);
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
