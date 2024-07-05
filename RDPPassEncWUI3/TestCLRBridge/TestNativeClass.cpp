#include "stdafx.h"

#include <Windows.h>
#include <process.h>
#include "TestNativeClass.h"
#include "ClrHelper.h"

using namespace System;
using namespace std;
using namespace sunjwbase;
using namespace WindowsComm;
using namespace TestCLRBridge;

int WINAPI SomeThreadFunc(void *param)
{
    TestNativeClass *pNativeClass = (TestNativeClass *)param;

    pNativeClass->SetTaskbarProg(1);

    for (int i = 0; i < 5; i++)
    {
        Sleep(1000);
        string strSome;
        strSome = strappendformat(strSome, "%d", (i + 1));
        pNativeClass->UpdateUI(strtotstr(strSome));
        pNativeClass->SetTaskbarProg((i + 1) * 15);
    }

    Sleep(1000);
    pNativeClass->UpdateUI(tstring(TEXT("DONE @#$@#$@#$")));
    pNativeClass->SetTaskbarProg(99);

    return 0;
}

TestNativeClass::TestNativeClass(TestManagedClass^ testManagedClass, String^ mstr) :
    m_testManagedClass(testManagedClass),
    m_tstrSome(ConvertToStdWstring(mstr)),
    m_hWorkThread(nullptr),
    m_hWnd(nullptr),
    m_pTaskbarList3(nullptr)
{
    m_tstrSome.append(TEXT(" 啊啊啊 "));
    InitTaskbarList3(&m_pTaskbarList3);
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
        (unsigned int (WINAPI *)(void *))SomeThreadFunc,
        this,
        0,
        (unsigned int *)&thredID);
}

void TestNativeClass::UpdateUI(const tstring& tstrSome)
{
    if (m_testManagedClass->UpdateUIHandler != nullptr)
    {
        String^ mstrSome = ConvertTstrToSystemString(tstrSome.c_str());
        m_testManagedClass->UpdateUIHandler(mstrSome);
    }
}

void TestNativeClass::SetHWND(IntPtr hWnd)
{
    m_hWnd = (HWND)hWnd.ToPointer();
}

void TestNativeClass::SetTaskbarProg(ULONGLONG ullValue)
{
    SetTaskbarList3Value(m_pTaskbarList3, m_hWnd, ullValue);
}
