#ifndef _DIPS_LIB_H_
#define _DIPS_LIB_H_

#if !defined(DIPSLIBAPI)
#define DIPSLIBAPI __declspec(dllimport)
#endif

// External function prototypes
DIPSLIBAPI BOOL WINAPI SetDIPSHook(DWORD dwThreadId);

#endif
