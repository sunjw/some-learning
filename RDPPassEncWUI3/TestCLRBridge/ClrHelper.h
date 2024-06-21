#pragma once
#include <string>
#include <Windows.h>

namespace TestCLRBridge
{
	std::string ConvertToStdString(System::String^ mstr);

	std::wstring ConvertToStdWstring(System::String^ mstr);

	System::String^ ConvertStrToSystemString(LPCSTR lpStrSource);

	System::String^ ConvertStrToSystemString(LPCWSTR lpWstrSource);
}
