#pragma once
#include <memory>
#include "strhelper.h"

class SomeNative
{
public:
	SomeNative(int someValue);
	virtual ~SomeNative() {}

	sunjwbase::tstring getHello();

private:
	std::shared_ptr<int> m_spTest;
};
