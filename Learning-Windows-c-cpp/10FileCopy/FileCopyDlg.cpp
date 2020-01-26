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

#include "IoCompletionPortHelper.h"
#include "IOReq.h"

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

#define BUFFSIZE				(64 * 1024)
#define MAX_PENDING_IO_REQS		4

#define CK_READ					1
#define CK_WRITE				2

using namespace std;
using namespace sunjwbase;

static HINSTANCE s_hInst = NULL; // Application instance handle
static HWND s_hDlg = NULL; // Dialog handle

template <class TV, class TM>
inline TV WINROUNDDOWN(TV Value, TM Multiple) {
	return((Value / Multiple) * Multiple);
}

template <class TV, class TM>
inline TV WINROUNDUP(TV Value, TM Multiple) {
	return(WINROUNDDOWN(Value, Multiple) +
		(((Value % Multiple) > 0) ? Multiple : 0));
}

BOOL FileCopy(PCTSTR pszFileSrc, PCTSTR pszFileDst)
{
	BOOL fOk = FALSE;
	LARGE_INTEGER liFileSizeSrc = { 0 }, liFileSizeDst;

	try
	{
		{
			// Open source file without buffering
			HANDLE hFileSrc = CreateFile(pszFileSrc, 
										GENERIC_READ, 
										FILE_SHARE_READ,
										NULL,
										OPEN_EXISTING,
										FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
										NULL);
			if (hFileSrc == INVALID_HANDLE_VALUE)
				goto leave;

			// Get file size
			GetFileSizeEx(hFileSrc, &liFileSizeSrc);

			// Nonbuffered I/O requires sector-sized transfers.
			liFileSizeDst.QuadPart = WINROUNDUP(liFileSizeSrc.QuadPart, BUFFSIZE);

			// Open destination file without buffering
			HANDLE hFileDst = CreateFile(pszFileDst,
										GENERIC_WRITE,
										0,
										NULL,
										CREATE_ALWAYS,
										FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
										hFileSrc);
			if (hFileDst == INVALID_HANDLE_VALUE)
				goto leave;

			// Set file system for asynchronously operations
			SetFilePointerEx(hFileDst, liFileSizeDst, NULL, FILE_BEGIN);
			SetEndOfFile(hFileDst);

			// Create an I/O completion port and associate the file
			CIOCP iocp(0);
			iocp.AssociateDevice(hFileSrc, CK_READ);
			iocp.AssociateDevice(hFileDst, CK_WRITE);

			// Initialize record-keeping
			CIOReq ior[MAX_PENDING_IO_REQS];
			LARGE_INTEGER liNextReadOffset = { 0 };
			int nReadsInProgress = 0;
			int nWritesInProgress = 0;

			// Begin with a fake CK_WRITE operations,
			// so that reader will go!
			for (int nIOReq = 0; nIOReq < _countof(ior); ++nIOReq)
			{
				// start each one
				ior[nIOReq].AllocBuffer(BUFFSIZE);
				++nWritesInProgress;
				iocp.PostStatus(CK_WRITE, 0, &ior[nIOReq]);
			}

			// Loop to go
			while (nReadsInProgress > 0 || nWritesInProgress > 0)
			{
				// Suspend until an I/O compltes
				ULONG_PTR CompletionKey;
				DWORD dwNumBytes;
				CIOReq *pior;
				iocp.GetStatus(&CompletionKey, 
								&dwNumBytes,
								(OVERLAPPED **)&pior,
								INFINITE);

				switch (CompletionKey)
				{
				case CK_READ:
					nReadsInProgress--;
					pior->Write(hFileDst);
					nWritesInProgress++;

					break;

				case CK_WRITE:
					nWritesInProgress--;
					if (liNextReadOffset.QuadPart < liFileSizeDst.QuadPart)
					{
						// Not EOF
						pior->Read(hFileSrc, &liNextReadOffset);
						nReadsInProgress++;
						liNextReadOffset.QuadPart += BUFFSIZE;
					}

					break;
				}
			}

			CloseHandle(hFileSrc);
			CloseHandle(hFileDst);

			fOk = TRUE;
		}
	leave:;
	}
	catch (...)
	{ }

	if (fOk)
	{
		// Shrink the destination file.
		HANDLE hFileDst = CreateFile(pszFileDst,
									GENERIC_WRITE,
									0,
									NULL,
									OPEN_EXISTING,
									0,
									NULL);
		if (hFileDst != INVALID_HANDLE_VALUE)
		{
			SetFilePointerEx(hFileDst, liFileSizeSrc, NULL, FILE_BEGIN);
			SetEndOfFile(hFileDst);

			CloseHandle(hFileDst);
		}
	}

	return fOk;
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
	EnableWindow(GetDlgItem(s_hDlg, IDC_BUTTON_COPY), FALSE);

}

static void OnCloseDialog(HWND hDlg, LPARAM lParam)
{
	
}

BOOL CALLBACK DlgMainProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR szPathName[_MAX_PATH];

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

		case IDC_BUTTON_COPY:
		{
			Static_GetText(GetDlgItem(s_hDlg, IDC_EDIT_PATH),
				szPathName,
				sizeof(szPathName));
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			BOOL bRet = FileCopy(szPathName, TEXT("FileCopy.cpy"));
			if (bRet)
			{
				MessageBox(GetActiveWindow(),
					TEXT("File Copy Successful!"),
					TEXT("FileCopy"),
					MB_OK);
			}
			else
			{
				MessageBox(GetActiveWindow(),
					TEXT("File Copy Failed!"),
					TEXT("FileCopy"),
					MB_OK);
			}

			return TRUE;
		}
		case IDC_BUTTON_OPEN:
			OPENFILENAME ofn = { OPENFILENAME_SIZE_VERSION_400 };
			ofn.hwndOwner = s_hDlg;
			ofn.lpstrFilter = TEXT("*.*\0");
			lstrcpy(szPathName, TEXT("*.*"));
			ofn.lpstrFile = szPathName;
			ofn.nMaxFile = _countof(szPathName);
			ofn.lpstrTitle = TEXT("Select file to copy");
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
			BOOL fOk = GetOpenFileName(&ofn);
			if (fOk)
			{
				// Display
				Static_SetText(GetDlgItem(s_hDlg, IDC_EDIT_PATH),
								szPathName);
				HANDLE hFile = CreateFile(szPathName,
											0,
											0,
											NULL,
											OPEN_EXISTING,
											0,
											NULL);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					LARGE_INTEGER liFileSize;
					GetFileSizeEx(hFile, &liFileSize);

					SetDlgItemInt(s_hDlg, IDC_EDIT_SIZE, liFileSize.LowPart, FALSE);
				}

				EnableWindow(GetDlgItem(s_hDlg, IDC_BUTTON_COPY), TRUE);
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
