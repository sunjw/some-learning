#pragma once
#include "stdafx.h"

#include <string>

#include "strhelper.h"

using namespace std;
using namespace sunjwbase;

inline string cstrtostr(const CString& cstring)
{
	return tstrtostr(cstring.GetString());
}

inline CString strtocstr(const string& str)
{
	CString temp;
	temp.SetString(strtotstr(str).c_str());
	return temp;
}
