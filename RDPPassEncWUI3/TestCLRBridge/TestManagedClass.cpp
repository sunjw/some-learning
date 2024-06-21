#include "stdafx.h"

#include "TestManagedClass.h"

#include <string>
#include "TestNativeClass.h"
#include "ClrHelper.h"

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
        String^ ret = ConvertWstrToSystemString(m_testNativeClass->GetString().c_str());
        ret = ret + " 10!";
        return ret;
    }

    void TestManagedClass::GoThread()
    {
        m_testNativeClass->GoThread();
    }
}
