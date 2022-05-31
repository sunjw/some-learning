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
	return tstring(L"将文件拖入或点击打开，开始计算。C++/CX native！");
}
