#include "pch.h"
#include "SomeNative.h"
#include "strhelper.h"

using namespace sunjwbase;

SomeNative::SomeNative(int someValue):
	m_spTest(new int(someValue))
{

}

tstring SomeNative::getHello()
{
	return tstring(L"���ļ���������򿪣���ʼ���㡣C++/CX native��");
}
