#include "stdafx.h"

#include "TestNativeClass.h"
#include "ClrHelper.h"

using namespace System;
using namespace TestCLRBridge;

TestNativeClass::TestNativeClass(TestManagedClass^ testManagedClass, String^ mstr):
	m_testManagedClass(testManagedClass), m_tstrSome(ConvertToStdWstring(mstr))
{
	m_tstrSome.append(TEXT(" 啊啊啊 "));
}
