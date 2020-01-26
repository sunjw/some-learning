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

static PTSTR BigNumToString(LONG lNum, PTSTR szBuf, DWORD chBufSize)
{
	TCHAR szNum[100];
	StringCchPrintf(szNum, _countof(szNum), TEXT("%d"), lNum);
	NUMBERFMT nf;
	nf.NumDigits = 0;
	nf.LeadingZero = FALSE;
	nf.Grouping = 3;
	nf.lpDecimalSep = TEXT(".");
	nf.lpThousandSep = TEXT(",");
	nf.NegativeOrder = 0;
	GetNumberFormat(LOCALE_USER_DEFAULT, 0, szNum, &nf, szBuf, chBufSize);

	return szBuf;
}

static void ShowCPUInfo(WORD wProcessorArchitecture,
						WORD wProcessorLevel,
						WORD wProcessorRevision)
{
	TCHAR szCPUArch[64] = TEXT("(unknown)");
	TCHAR szCPULevel[64] = TEXT("(unknown)");
	TCHAR szCPURev[64] = TEXT("(unknown)");

	switch (wProcessorArchitecture)
	{
		// Not a real detection, just for fun.
	case PROCESSOR_ARCHITECTURE_INTEL:
		_tcscpy_s(szCPUArch, _countof(szCPUArch), TEXT("Intel"));
		switch (wProcessorLevel)
		{
		case 3:
		case 4:
			StringCchPrintf(szCPULevel, _countof(szCPULevel),
				TEXT("80%c86"), wProcessorLevel+'0');
			StringCchPrintf(szCPURev, _countof(szCPURev),
				TEXT("%c%d"), 
				HIBYTE(wProcessorRevision) + TEXT('A'),
				LOBYTE(wProcessorRevision));

			break;
		case 5:
			_tcscpy_s(szCPULevel, _countof(szCPULevel), TEXT("Pentium"));
			StringCchPrintf(szCPURev, _countof(szCPURev),
				TEXT("Model %d, Stepping %d"),
				HIBYTE(wProcessorRevision),
				LOBYTE(wProcessorRevision));

			break;
		case 6:
			switch (HIBYTE(wProcessorRevision))
			{
			case 1:
				_tcscpy_s(szCPULevel, _countof(szCPULevel), TEXT("Pentium Pro"));
				break;
			case 3:
			case 5:
				_tcscpy_s(szCPULevel, _countof(szCPULevel), TEXT("Pentium II"));
				break;
			case 6:
				_tcscpy_s(szCPULevel, _countof(szCPULevel), TEXT("Celeron"));
				break;
			case 7:
			case 8:
			case 11:
				_tcscpy_s(szCPULevel, _countof(szCPULevel), TEXT("Pentium III"));
				break;
			case 9:
			case 13:
				_tcscpy_s(szCPULevel, _countof(szCPULevel), TEXT("Pentium M"));
				break;
			case 10:
				_tcscpy_s(szCPULevel, _countof(szCPULevel), TEXT("Pentium Xeon"));
				break;
			case 15:
				_tcscpy_s(szCPULevel, _countof(szCPULevel), TEXT("Core 2 Duo"));
				break;
			default:
				_tcscpy_s(szCPULevel, _countof(szCPULevel), TEXT("Unknown Pentium"));
				break;
			} // switch (HIBYTE(wProcessorRevision)) end

			StringCchPrintf(szCPURev, _countof(szCPURev),
				TEXT("Model %d, Stepping %d"),
				HIBYTE(wProcessorRevision),
				LOBYTE(wProcessorRevision));

			break; // case 6 end
		case 15:
			_tcscpy_s(szCPULevel, _countof(szCPULevel), TEXT("Pentium 4"));
			StringCchPrintf(szCPURev, _countof(szCPURev),
				TEXT("Model %d, Stepping %d"),
				HIBYTE(wProcessorRevision),
				LOBYTE(wProcessorRevision));

			break;
		} // switch (wProcessorLevel) end

		break; // case PROCESSOR_ARCHITECTURE_INTEL end
	case PROCESSOR_ARCHITECTURE_IA64:
		_tcscpy_s(szCPUArch, _countof(szCPUArch), TEXT("IA-64"));
		StringCchPrintf(szCPULevel, _countof(szCPULevel),
			TEXT("%d"), wProcessorLevel);
		StringCchPrintf(szCPURev, _countof(szCPURev),
			TEXT("Model %c, Pass %d"), 
			HIBYTE(wProcessorRevision)+TEXT('A'),
			LOBYTE(wProcessorRevision));

		break; // case PROCESSOR_ARCHITECTURE_IA64 end
	case PROCESSOR_ARCHITECTURE_AMD64:
		_tcscpy_s(szCPUArch, _countof(szCPUArch), TEXT("AMD64"));
		StringCchPrintf(szCPULevel, _countof(szCPULevel),
			TEXT("%d"), wProcessorLevel);
		StringCchPrintf(szCPURev, _countof(szCPURev),
			TEXT("Model %c, Pass %d"),
			HIBYTE(wProcessorRevision) + TEXT('A'),
			LOBYTE(wProcessorRevision));

		break; // case PROCESSOR_ARCHITECTURE_AMD64 end
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
	default:
		_tcscpy_s(szCPUArch, _countof(szCPUArch), TEXT("Unknown"));

		break; // case PROCESSOR_ARCHITECTURE_UNKNOWN end
	} // switch (wProcessorArchitecture) end

	SetDlgItemText(s_hDlg, IDC_STATIC_PA, szCPUArch);
	SetDlgItemText(s_hDlg, IDC_STATIC_PL, szCPULevel);
	SetDlgItemText(s_hDlg, IDC_STATIC_PR, szCPURev);
}

static void ShowBitness()
{
	TCHAR szFullTitle[100];
	TCHAR szTitle[32];

	GetWindowText(s_hDlg, szTitle, _countof(szTitle));

#ifndef _WIN64
	BOOL bIsWow64 = FALSE;
	if(!IsWow64Process(GetCurrentProcess(), &bIsWow64))
	{
		return; // failed
	}

	if (bIsWow64)
	{
		StringCchPrintf(szFullTitle, _countof(szFullTitle),
			TEXT("32-bit %s on WOW64"), szTitle);
	}
	else
	{
		StringCchPrintf(szFullTitle, _countof(szFullTitle),
			TEXT("32-bit %s on 32-bit Windows"), szTitle);
	}
#else
	StringCchPrintf(szFullTitle, _countof(szFullTitle),
		TEXT("64-bit %s"), szTitle);
#endif

	SetWindowText(s_hDlg, szFullTitle);
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
	SYSTEM_INFO sinf;
	GetSystemInfo(&sinf);

	ShowCPUInfo(sinf.wProcessorArchitecture,
				sinf.wProcessorLevel,
				sinf.wProcessorRevision);

	TCHAR szBuf[50];

	SetDlgItemText(s_hDlg, IDC_STATIC_PS, 
		BigNumToString(sinf.dwPageSize, szBuf, _countof(szBuf)));

	StringCchPrintf(szBuf, _countof(szBuf),
		TEXT("%p"), sinf.lpMinimumApplicationAddress);
	SetDlgItemText(s_hDlg, IDC_STATIC_MINAA, szBuf);

	StringCchPrintf(szBuf, _countof(szBuf),
		TEXT("%p"), sinf.lpMaximumApplicationAddress);
	SetDlgItemText(s_hDlg, IDC_STATIC_MAXAA, szBuf);

	StringCchPrintf(szBuf, _countof(szBuf),
		TEXT("0x%016I64X"), 
		(__int64)sinf.dwActiveProcessorMask);
	SetDlgItemText(s_hDlg, IDC_STATIC_APM, szBuf);

	SetDlgItemText(s_hDlg, IDC_STATIC_NOP,
		BigNumToString(sinf.dwNumberOfProcessors, szBuf, _countof(szBuf)));

	SetDlgItemText(s_hDlg, IDC_STATIC_AG,
		BigNumToString(sinf.dwAllocationGranularity, szBuf, _countof(szBuf)));

	ShowBitness();
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
