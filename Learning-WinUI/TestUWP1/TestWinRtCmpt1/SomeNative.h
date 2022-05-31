#pragma once
#include <memory>
#include <Windows.h>
#include "TestDelegate.h"
#include "strhelper.h"

class SomeNative
{
public:
	SomeNative(TestUWP1::TestDelegate^ someDelegate);
	virtual ~SomeNative();

	void getHello();
	void doGetHello();

private:
	TestUWP1::TestDelegate^ m_testDelegate;
	HANDLE m_thrdHandle;
};
