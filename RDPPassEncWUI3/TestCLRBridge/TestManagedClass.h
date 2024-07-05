#pragma once
#include "strhelper.h"

namespace TestCLRBridge
{
    class TestNativeClass;

    public delegate void UpdateThreadHandler(bool running);
    public delegate void UpdateUIHandler(System::String^ someStr);

    public ref class TestManagedClass
    {
    public:
        TestManagedClass(System::String^ someStr);

        virtual ~TestManagedClass()
        {
            // clean up code to release managed resource
            // ...
            // to avoid code duplication,
            // call finalizer to release unmanaged resources
            this->!TestManagedClass();
        }

        // finalizer cleans up unmanaged resources
        // destructor or garbage collector will
        // clean up managed resources
        !TestManagedClass();

        System::String^ GetTestValue();
        System::String^ GetWindowsInfo();

        void GoThread();
        void StopThread();

        void SetHWND(System::IntPtr hWnd);

        UpdateThreadHandler^ UpdateThreadHandler;
        UpdateUIHandler^ UpdateUIHandler;

    private:
        TestNativeClass *m_testNativeClass;
    };
}
