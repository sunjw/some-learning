#include "stdafx.h"

#include <Windows.h>
#include <process.h>
#include "TestNativeClass.h"
#include "ClrHelper.h"

using namespace System;
using namespace TestCLRBridge;

int WINAPI SomeThreadFunc(void *param)
{
    return 0;
}

TestNativeClass::TestNativeClass(TestManagedClass^ testManagedClass, String^ mstr):
    m_testManagedClass(testManagedClass),
    m_tstrSome(ConvertToStdWstring(mstr)),
    m_hWorkThread(NULL)
{
    m_tstrSome.append(TEXT(" 啊啊啊 "));
}

void TestNativeClass::GoThread()
{
    if (m_hWorkThread)
    {
        CloseHandle(m_hWorkThread);
    }
    DWORD thredID;
    m_hWorkThread = (HANDLE)_beginthreadex(NULL,
        0,
        (unsigned int (WINAPI*)(void*))SomeThreadFunc,
        NULL,
        0,
        (unsigned int*)&thredID);
}
