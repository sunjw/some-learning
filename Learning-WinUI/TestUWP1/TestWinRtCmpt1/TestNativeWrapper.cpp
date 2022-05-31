#include "pch.h"
#include "TestNativeWrapper.h"
#include "strhelper.h"
#include "TestDelegate.h"

using namespace Platform;
using namespace sunjwbase;
using namespace TestUWP1;

TestNativeWrapper::TestNativeWrapper(TestDelegate^ testDelegate)
{
	m_testDelegate = testDelegate;
	m_spSomeNative.reset(new SomeNative(m_testDelegate));
}

void TestNativeWrapper::GetHello()
{
	m_spSomeNative->getHello();
}
