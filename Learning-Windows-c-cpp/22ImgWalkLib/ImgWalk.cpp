#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <string>

#include "tchar.h"
#include "process.h"
#include "windows.h"

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, PVOID fImpLoad)
{
	if(fdwReason == DLL_PROCESS_ATTACH)
	{
		char szBuf[MAX_PATH * 100] = {0};

		PBYTE pb = NULL;
		MEMORY_BASIC_INFORMATION mbi;

		while(VirtualQuery(pb, &mbi, sizeof(mbi)) == sizeof(mbi))
		{
			int nLen;
			char szModName[MAX_PATH];

			if(mbi.State = MEM_FREE)
				mbi.AllocationBase = mbi.BaseAddress;

			if(mbi.AllocationBase == hInstDll ||
				mbi.AllocationBase != mbi.BaseAddress ||
				mbi.AllocationBase == NULL)
				nLen = 0;
			else
				nLen = GetModuleFileNameA((HINSTANCE)mbi.AllocationBase,
							szModName, _countof(szModName));

			if(nLen > 0)
			{
				wsprintfA(strchr(szBuf, 0), "\r\n%p-%s",
					mbi.AllocationBase, szModName);
			}

			pb += mbi.RegionSize;
		}

		MessageBoxA(NULL, 
				&szBuf[1],
				"ImgWalk",
				MB_OK | MB_ICONINFORMATION);
	}

	return TRUE;
}
