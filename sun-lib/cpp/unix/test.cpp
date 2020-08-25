/*
 * test.cpp
 *
 *  Created on: 2011-9-13
 *      Author: sun
 */
#include <iostream>

#include "SimpleHttp.h"
#include "strhelper.h"

using namespace std;
using namespace sunjwbase;

int main()
{
	SimpleHttp http;

	string strUrl("http://www.google.com/?oe=gbk");
	string strRet;
	http.DoGet(strUrl, strRet, false);
	//cout << strRet << endl;

	strRet = utf8conv(strRet);
	cout << strRet << endl;

	strRet = asciiconv(strRet);
	//cout << strRet << endl;
    
	return 0;
}
