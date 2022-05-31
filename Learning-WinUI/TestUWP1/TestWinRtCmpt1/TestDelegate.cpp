#include "pch.h"
#include "TestDelegate.h"

using namespace Platform;
using namespace TestUWP1;

TestDelegate::TestDelegate()
{

}

void TestDelegate::OnGetHello(String^ strHello)
{
	OnHelloHandler(strHello);
}
