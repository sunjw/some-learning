#pragma once
#include "strhelper.h"

namespace TestCLRBridge
{
    public ref class TestClass
    {
    public:
        TestClass(System::String^ someStr);
        ~TestClass() 
        {
            // clean up code to release managed resource
            // ...
            // to avoid code duplication,
            // call finalizer to release unmanaged resources
            this->!TestClass();
        }

        // finalizer cleans up unmanaged resources
        // destructor or garbage collector will
        // clean up managed resources
        !TestClass();

        System::String^ GetTestValue();

    private:
        sunjwbase::tstring *m_tstrSomeStr;
    };
}
