#include "stdafx.h"

#include "TestManagedClass.h"

#include <string>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

#include "TestNativeClass.h"

using namespace System;

using namespace std;
using namespace msclr::interop;
using namespace sunjwbase;

namespace TestCLRBridge
{
    TestManagedClass::TestManagedClass(String^ someStr):
        m_tstrSomeStr(new tstring(marshal_as<tstring>(someStr))),
        m_testNativeClass(new TestNativeClass(this))
    {

    }

    TestManagedClass::!TestManagedClass()
    {
        delete m_tstrSomeStr;
    }

    String^ TestManagedClass::GetTestValue()
    {
        m_tstrSomeStr->append(TEXT(" 100!"));
        return marshal_as<String^>(m_tstrSomeStr->c_str());
    }
}
