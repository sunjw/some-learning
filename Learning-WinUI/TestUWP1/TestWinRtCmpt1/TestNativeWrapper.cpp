#include "pch.h"
#include "TestNativeWrapper.h"

using namespace TestUWP1;
using namespace Platform;

TestNativeWrapper::TestNativeWrapper()
{
}

String^ TestNativeWrapper::GetHello()
{
	return "将文件拖入或点击打开，开始计算。C++/CX！";
}
