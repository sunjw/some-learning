#include "stdafx.h"

#include "TestManagedClass.h"

#include <string>
#include "TestNativeClass.h"
#include "ClrHelper.h"
#include "WindowsComm.h"

using namespace System;

using namespace std;
using namespace sunjwbase;

namespace TestCLRBridge
{
    TestManagedClass::TestManagedClass(String^ someStr):
        m_testNativeClass(new TestNativeClass(this, someStr))
    {
    }

    TestManagedClass::!TestManagedClass()
    {
        delete m_testNativeClass;
    }

    String^ TestManagedClass::GetTestValue()
    {
        String^ ret = ConvertTstrToSystemString(m_testNativeClass->GetString().c_str());
        ret = ret + " 10!";
        return ret;
    }

    String^ TestManagedClass::GetWindowsInfo()
    {
        tstring tstrWinInfo = WindowsComm::GetWindowsInfo();
        tstrWinInfo = strtrim(tstrWinInfo);
        String^ mstrWinInfo = ConvertTstrToSystemString(tstrWinInfo.c_str());
        return mstrWinInfo;
    }

    void TestManagedClass::GoThread()
    {
        m_testNativeClass->GoThread();
    }

    void TestManagedClass::StopThread()
    {
        m_testNativeClass->StopThread();
    }

    void TestManagedClass::SetHWND(IntPtr hWnd)
    {
        m_testNativeClass->SetHWND(hWnd);
    }
}
