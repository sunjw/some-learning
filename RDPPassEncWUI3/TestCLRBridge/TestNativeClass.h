#pragma once
#include <Windows.h>
#include <msclr\auto_gcroot.h>
#include "strhelper.h"
#include "TestManagedClass.h"
#include "AdvTaskbar.h"

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
        void StopThread();
        inline bool GetReqStop() const
        {
            return m_bReqStop;
        }

        void UpdateThread(bool running);
        void UpdateUI(const sunjwbase::tstring& tstrSome);

        void SetHWND(System::IntPtr hWnd);
        void SetTaskbarProg(ULONGLONG ullValue);

    private:
        msclr::auto_gcroot<TestManagedClass^> m_testManagedClass;
        sunjwbase::tstring m_tstrSome;
        HWND m_hWnd;
        ITaskbarList3 *m_pTaskbarList3;
        HANDLE m_hWorkThread;
        bool m_bReqStop;
    };
}
