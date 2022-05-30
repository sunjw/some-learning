#pragma once

namespace TestUWP1
{
    public ref class TestNativeWrapper sealed
    {
    public:
        TestNativeWrapper();

        Platform::String^ GetHello();
    };
}
