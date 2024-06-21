#include "stdafx.h"

#include "TestNativeClass.h"

using namespace TestCLRBridge;

TestNativeClass::TestNativeClass(TestManagedClass^ testManagedClass)
{
	m_testManagedClass = testManagedClass;
}
