#pragma once
#include <memory>
#include "TestDelegate.h"
#include "strhelper.h"

class SomeNative
{
public:
	SomeNative(TestUWP1::TestDelegate^ someDelegate);
	virtual ~SomeNative() {}

	void getHello();

private:
	TestUWP1::TestDelegate^ m_testDelegate;
};
