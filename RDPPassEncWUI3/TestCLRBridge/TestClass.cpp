#pragma once
#include "stdafx.h"

#include "TestClass.h"

#include <string>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

using namespace System;

using namespace std;
using namespace msclr::interop;
using namespace sunjwbase;

namespace TestCLRBridge
{
    TestClass::TestClass(String^ someStr):
        m_tstrSomeStr(new tstring(marshal_as<tstring>(someStr)))
    {

    }

    TestClass::!TestClass()
    {
        delete m_tstrSomeStr;
    }

    String^ TestClass::GetTestValue()
    {
        m_tstrSomeStr->append(TEXT(" 100!"));
        return marshal_as<String^>(m_tstrSomeStr->c_str());
    }
}
