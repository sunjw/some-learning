#include "stdafx.h"

#include <Windows.h>
#include <process.h>
#include "TestNativeClass.h"
#include "ClrHelper.h"

using namespace System;
using namespace std;
using namespace sunjwbase;
using namespace TestCLRBridge;

int WINAPI SomeThreadFunc(void *param)
{
    TestNativeClass *pNativeClass = (TestNativeClass *)param;

    for (int i = 0; i < 3; i++)
    {
        Sleep(1000);
        string strSome;
        strSome = strappendformat(strSome, "%d", (i + 1));
        pNativeClass->UpdateUI(strtotstr(strSome));
    }

    Sleep(1000);
    pNativeClass->UpdateUI(tstring(TEXT("DONE @#$@#$@#$")));

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
        this,
        0,
        (unsigned int*)&thredID);
}

void TestNativeClass::UpdateUI(const tstring& tstrSome)
{
    String^ mstrSome = ConvertWstrToSystemString(tstrSome.c_str());
    m_testManagedClass->UpdateUIHandler(mstrSome);
}
