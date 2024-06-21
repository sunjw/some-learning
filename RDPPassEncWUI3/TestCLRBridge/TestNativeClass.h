#pragma once
#include <msclr\auto_gcroot.h>
#include "strhelper.h"
#include "TestManagedClass.h"

namespace TestCLRBridge
{
	class TestNativeClass
	{
	public:
		TestNativeClass(TestManagedClass^ testManagedClass);

	private:
		msclr::auto_gcroot<TestManagedClass^> m_testManagedClass;
		sunjwbase::tstring m_tstrSome;
	};
}
