#include "pch.h"
#include "SomeNative.h"
#include "TestDelegate.h"
#include "strhelper.h"

using namespace Platform;
using namespace sunjwbase;
using namespace TestUWP1;

SomeNative::SomeNative(TestDelegate^ someDelegate):
	m_testDelegate(someDelegate)
{

}

void SomeNative::getHello()
{
	if (m_testDelegate == nullptr)
	{
		return;
	}
	tstring tstrHello(L"���ļ���������򿪣���ʼ���㡣C++/CX native��");
	String^ strHello = ref new String(tstrHello.c_str());
	m_testDelegate->OnGetHello(strHello);
}
