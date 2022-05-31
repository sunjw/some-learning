#pragma once

namespace TestUWP1
{
	public delegate void OnHelloHandler(Platform::String^ strHello);

    public ref class TestDelegate sealed
    {
    public:
        TestDelegate();

        void OnGetHello(Platform::String^ strHello);

        event OnHelloHandler^ OnHelloHandler;
    };
}
