#pragma once
#include "strhelper.h"

namespace TestCLRBridge
{
    class TestNativeClass;

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

    private:
        TestNativeClass *m_testNativeClass;
    };
}
