#include "pch.h"
#include "TestNativeWrapper.h"
#include "strhelper.h"

using namespace TestUWP1;
using namespace Platform;
using namespace sunjwbase;

TestNativeWrapper::TestNativeWrapper()
{
	m_spSomeNative.reset(new SomeNative(100));
}

String^ TestNativeWrapper::GetHello()
{
	tstring tstrHello = m_spSomeNative->getHello();
	String^ strHello = ref new String(tstrHello.c_str());
	return strHello;
}
