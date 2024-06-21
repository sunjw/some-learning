#pragma once
#include <Windows.h>
#include <msclr\auto_gcroot.h>
#include "strhelper.h"
#include "TestManagedClass.h"

namespace TestCLRBridge
{
    class TestNativeClass
    {
    public:
        TestNativeClass(TestManagedClass^ testManagedClass, System::String^ mstr);

        inline sunjwbase::tstring GetString() const
        {
            return m_tstrSome;
        }

        void GoThread();
        void UpdateUI(const sunjwbase::tstring& tstrSome);

    private:
        msclr::auto_gcroot<TestManagedClass^> m_testManagedClass;
        sunjwbase::tstring m_tstrSome;
        HANDLE m_hWorkThread;
    };
}
