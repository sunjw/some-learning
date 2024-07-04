#include "stdafx.h"

#include "strhelper.h"
#include "WindowsComm.h"

using namespace std;
using namespace sunjwbase;
using namespace WindowsComm;

int wmain(int argc, wchar_t* argv[])
{
    tstring tstrWindowsInfo = GetWindowsInfo();
    wprintf(L"%s\r\n", tstrWindowsInfo.c_str());

    return 0;
}
