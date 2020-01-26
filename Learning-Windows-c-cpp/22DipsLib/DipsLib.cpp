#include <stdlib.h>
#include <string.h>

#include <string>

#include "tchar.h"
#include "process.h"
#include "windows.h"
#include "Windowsx.h"
#include "Strsafe.h"
#include "Commctrl.h"

//#include "CommCtrlUI.h"
#include "strhelper.h"
#include "resource.h"

#define DIPSLIBAPI __declspec(dllexport)
#include "DipsLib.h"

#ifdef _DEBUG
void ForceDebugBreak()
{
	__try
	{ DebugBreak(); }
	__except(UnhandledExceptionFilter(GetExceptionInformation()))
	{}
}
#else
#define ForceDebugBreak()
#endif

LRESULT WINAPI GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);

INT_PTR WINAPI Dlg_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


#pragma data_seg("Shared")
HHOOK g_hHook = NULL;
DWORD g_dwThreadIdDips = 0;
#pragma data_seg()

#pragma comment(linker, "/section:Shared,rws")


HINSTANCE g_hInstDll = NULL;

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, PVOID fImpLoad)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hInstDll = hInstDll;
		break;

	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

BOOL WINAPI SetDIPSHook(DWORD dwThreadId)
{
	BOOL bOk = FALSE;

	if(dwThreadId != 0)
	{
		if(g_hHook != NULL)
			return FALSE;

		g_dwThreadIdDips = GetCurrentThreadId();

		g_hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, g_hInstDll, 0);

		bOk = (g_hHook != NULL);
		if(bOk)
		{
			bOk = PostThreadMessage(dwThreadId, WM_NULL, 0, 0);
		}
	}
	else
	{
		if(g_hHook == NULL)
			return FALSE;
		
		bOk = UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
	}

	return bOk;
}

LRESULT WINAPI GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	static BOOL bFirstTime = TRUE;

	if(bFirstTime)
	{
		bFirstTime = FALSE;

		CreateDialog(g_hInstDll, MAKEINTRESOURCE(IDD_DIPS), NULL, Dlg_Proc);

		// Post a dummy message
		PostThreadMessage(g_dwThreadIdDips, WM_NULL, 0, 0);
	}

	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

static const TCHAR g_szRegSubKey[] = TEXT("Software\\Wintellect\\DIPS");

void SaveListViewItemPositions(HWND hWndLv)
{
	int nMaxItems = ListView_GetItemCount(hWndLv);

	// Delete it first
	LONG l = RegDeleteKey(HKEY_CURRENT_USER, g_szRegSubKey);

	HKEY hkey;
	l = RegCreateKeyEx(HKEY_CURRENT_USER, g_szRegSubKey, 0, 
		NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hkey, NULL);

	if(l != ERROR_SUCCESS)
		return;

	for(int nItem = 0; nItem < nMaxItems; ++nItem)
	{
		TCHAR szName[MAX_PATH] = {0};
		ListView_GetItemText(hWndLv, nItem, 0, szName, _countof(szName));

		POINT pt;
		ListView_GetItemPosition(hWndLv, nItem, &pt);

		if(szName[0] != 0)
		{
			l = RegSetValueEx(hkey, szName, 0, REG_BINARY, (PBYTE)&pt, sizeof(pt));

			if(l != ERROR_SUCCESS)
				return;
		}
	}

	RegCloseKey(hkey);
}

void RestoreListViewItemPositions(HWND hWndLv)
{
	HKEY hkey;
	LONG l = RegOpenKeyEx(HKEY_CURRENT_USER, g_szRegSubKey,
		0, KEY_QUERY_VALUE, &hkey);
	if(l == ERROR_SUCCESS)
	{
		DWORD dwStyle = GetWindowStyle(hWndLv);
		if(dwStyle & LVS_AUTOARRANGE)
			SetWindowLong(hWndLv, GWL_STYLE, dwStyle & ~LVS_AUTOARRANGE);

		l = NO_ERROR;
		for(int nIndex = 0; l != ERROR_NO_MORE_ITEMS; ++nIndex)
		{
			TCHAR szName[MAX_PATH];
			DWORD cbValueName = _countof(szName);

			POINT pt;
			DWORD cbData = sizeof(pt), nItem;

			DWORD dwType;
			l = RegEnumValue(hkey, nIndex, szName, &cbValueName,
				NULL, &dwType, (PBYTE)&pt, &cbData);

			if(l == ERROR_NO_MORE_ITEMS)
				continue;

			if((dwType == REG_BINARY) && (cbData == sizeof(pt)))
			{
				LV_FINDINFO lvfi;
				lvfi.flags = LVFI_STRING;
				lvfi.psz = szName;
				nItem = ListView_FindItem(hWndLv, -1, &lvfi);
				if(nItem != -1)
				{
					ListView_SetItemPosition(hWndLv, nItem, pt.x, pt.y);
				}
			}
		}

		SetWindowLong(hWndLv, GWL_STYLE, dwStyle);

		RegCloseKey(hkey);
	}
}

INT_PTR WINAPI Dlg_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return TRUE;

	case WM_APP:
		if(lParam)
			SaveListViewItemPositions((HWND)wParam);
		else
			RestoreListViewItemPositions((HWND)wParam);

		break;

	// other message

	}

	return FALSE;
}
