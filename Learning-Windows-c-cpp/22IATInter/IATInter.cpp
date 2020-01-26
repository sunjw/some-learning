#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <string>

#include "tchar.h"
#include "process.h"
#include "windows.h"

#include "resource.h"
#include "CommCtrlUI.h"
#include "strhelper.h"

using namespace std;
using namespace sunjwbase;

void __stdcall NewMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    //printf("The String Sent to Message Box was :%s",lpText);

	MessageBoxW(NULL, L"MessageBoxW not MessageBoxA", L"MessageBoxW", 0);

    return;
}

void** IATfind(const char* function, HMODULE module)
{
	int ip = 0;
	if (module == 0)
		module = GetModuleHandle(0);
	PIMAGE_DOS_HEADER pImgDosHeaders = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS pImgNTHeaders = 
		(PIMAGE_NT_HEADERS)((LPBYTE)pImgDosHeaders + pImgDosHeaders->e_lfanew);
	PIMAGE_IMPORT_DESCRIPTOR pImgImportDesc = 
		(PIMAGE_IMPORT_DESCRIPTOR)
		((LPBYTE)pImgDosHeaders + 
		pImgNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	if (pImgDosHeaders->e_magic != IMAGE_DOS_SIGNATURE)
		printf("libPE Error : e_magic is no valid DOS signature\n");

	for (IMAGE_IMPORT_DESCRIPTOR* iid = pImgImportDesc; iid->Name != NULL; iid++)
	{
		for (int funcIdx = 0; 
			*(funcIdx + (LPVOID*)(iid->FirstThunk + (SIZE_T)module)) != NULL; 
			funcIdx++)
		{
			char* modFuncName = (char*)
				(*(funcIdx + 
				(SIZE_T*)(iid->OriginalFirstThunk + 
				(SIZE_T)module)) + 
				(SIZE_T)module + 2);
			const uintptr_t nModFuncName = (uintptr_t)modFuncName;
			bool isString = 
				!(nModFuncName & (sizeof(nModFuncName) == 4 ? 0x80000000 : 0x8000000000000000));
			if (isString)
			{
				if (!_stricmp(function, modFuncName))
					return funcIdx + (LPVOID*)(iid->FirstThunk + (SIZE_T)module);
			}
		}
	}
	return 0;
}
 
void DetourIATptr(const char* function, void* newfunction, HMODULE module)
{
	void** funcptr = IATfind(function, module);
	if (*funcptr == newfunction)
		return;

	DWORD oldrights, newrights = PAGE_READWRITE;
	VirtualProtect(funcptr, sizeof(LPVOID), newrights, &oldrights);
	//oldfunctionptr = *funcptr;
	*funcptr = newfunction;
	VirtualProtect(funcptr, sizeof(LPVOID), oldrights, &newrights);
}

int WINAPI _tWinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine,
                   int nCmdShow)
{
	DetourIATptr("MessageBoxA", (void*)NewMessageBoxA, 0);
	MessageBoxA(NULL, "aa", "aa", 0);

	return 0;
}
