#pragma once
#include <memory>
#include "SomeNative.h"

namespace TestUWP1
{
    public ref class TestNativeWrapper sealed
    {
    public:
        TestNativeWrapper();

        Platform::String^ GetHello();

    private:
        std::shared_ptr<SomeNative> m_spSomeNative;
    };
}
