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

#define FILENAME TEXT("FileRev.dat")

using namespace std;
using namespace sunjwbase;

static HINSTANCE s_hInst = NULL; // Application instance handle
static HWND s_hDlg = NULL; // Dialog handle

void ErrMsgBox(PCTSTR pszText)
{
	MessageBox(s_hDlg, 
				pszText,
				TEXT("Error"),
				MB_OK | MB_ICONERROR);
}

BOOL FileReverse(PCTSTR pszPathname, PBOOL pbIsTextUnicode)
{
	*pbIsTextUnicode = FALSE; // Assume not Unicode

	HANDLE hFile = CreateFile(pszPathname, 
							GENERIC_WRITE | GENERIC_READ,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		ErrMsgBox(TEXT("File could not be opened."));
		return FALSE;
	}

	// Get file size
	DWORD dwFileSize = GetFileSize(hFile, NULL);

	// Create file mapping object.
	HANDLE hFileMap = CreateFileMapping(hFile, 
										NULL,
										PAGE_READWRITE,
										0,
										dwFileSize + sizeof(WCHAR), NULL);

	if(hFileMap == NULL)
	{
		ErrMsgBox(TEXT("File map could not be opened."));
		return FALSE;
	}

	PVOID pvFile = MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);

	if(pvFile == NULL)
	{
		ErrMsgBox(TEXT("Could not map view of file."));

		CloseHandle(hFileMap);
		CloseHandle(hFile);

		return FALSE;
	}

	// Let's guess if file is Unicode.
	int iUnicodeTestFlags = -1; // Try all tests
	*pbIsTextUnicode = IsTextUnicode(pvFile, dwFileSize, &iUnicodeTestFlags);

	if(!*pbIsTextUnicode)
	{
		// ASCII
		PSTR pchANSI = (PSTR)pvFile;
		pchANSI[dwFileSize/sizeof(CHAR)] = 0;
		
		_strrev(pchANSI);

		// Reverse \n\r to \r\n
		pchANSI = strstr(pchANSI, "\n\r");
		while(pchANSI != NULL)
		{
			*pchANSI++ = '\r';
			*pchANSI++ = '\n';
			pchANSI = strstr(pchANSI, "\n\r");
		}
	}
	else
	{
		// Unicode
		PWSTR pchUnicode = (PWSTR)pvFile;
		pchUnicode[dwFileSize/sizeof(WCHAR)] = 0;

		if((iUnicodeTestFlags & IS_TEXT_UNICODE_SIGNATURE) != 0)
		{
			// Jump over signature
			pchUnicode++;
		}

		_wcsrev(pchUnicode);

		pchUnicode = wcsstr(pchUnicode, L"\n\r");
		while(pchUnicode != NULL)
		{
			*pchUnicode++ = L'\r';
			*pchUnicode++ = L'\n';
			pchUnicode = wcsstr(pchUnicode, L"\n\r");
		}
	}

	UnmapViewOfFile(pvFile);
	CloseHandle(hFileMap);

	SetFilePointer(hFile, dwFileSize, NULL, FILE_BEGIN);
	SetEndOfFile(hFile);
	CloseHandle(hFile);

	return TRUE;
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
	TCHAR szPathname[MAX_PATH];

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

		case IDC_BUTTON_OPEN:
			{
				OPENFILENAME ofn = { OPENFILENAME_SIZE_VERSION_400 };
				ofn.hwndOwner = hDlg;
				ofn.lpstrFile = szPathname;
				ofn.lpstrFile[0] = 0;
				ofn.nMaxFile = _countof(szPathname);
				ofn.lpstrTitle = TEXT("Select file for reversing");
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
				
				GetOpenFileName(&ofn);
				SetDlgItemText(hDlg, IDC_EDIT_PATH, ofn.lpstrFile);

				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_REVERSE), TRUE);
				SetFocus(GetDlgItem(hDlg, IDC_BUTTON_REVERSE));
			}

			return TRUE;

		case IDC_BUTTON_REVERSE:
			GetDlgItemText(hDlg, IDC_EDIT_PATH, szPathname, _countof(szPathname));
			if(!CopyFile(szPathname, FILENAME, FALSE)) 
			{
				ErrMsgBox(TEXT("New file could not be created."));
				return TRUE;
			}

			BOOL bIsTextUnicode;
			if(FileReverse(FILENAME, &bIsTextUnicode))
			{
				SetDlgItemText(hDlg, IDC_STATIC_ENCODE,
					bIsTextUnicode ? TEXT("Unicode") : TEXT("ANSI"));

				// Spawn Notepad
				STARTUPINFO si = { sizeof(si) };
				PROCESS_INFORMATION pi;
				TCHAR sz[] = TEXT("Notepad ") FILENAME;
				if(CreateProcess(NULL, sz, NULL, NULL,
								FALSE, 0, NULL, NULL,
								&si, &pi))
				{
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
				}
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
