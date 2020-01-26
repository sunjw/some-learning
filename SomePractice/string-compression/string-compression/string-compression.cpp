// string-compression.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <stdio.h>
#include <iostream>
#include <string>

#pragma warning(disable:4996)

using namespace std;

string compress(string& originalString) 
{
	string ret;

	size_t strLen = originalString.length();

	char lastChar = 0;
	int lastCharCount = 0;
	for (size_t i = 0; i <= strLen; ++i)
	{
		char ch = originalString[i];
		if (ch == lastChar)
		{
			lastCharCount++;
		}
		else
		{
			if (lastChar != 0)
			{
				ret += lastChar;
				char buf[256] = { 0 };
				sprintf(buf, "%d", lastCharCount);
				ret += string(buf);
			}
			lastChar = ch;
			lastCharCount = 1;
		}
	}

	if (ret.length() >= strLen)
		ret = originalString;

	return ret;
}

int main()
{
	// https://www.lintcode.com/problem/string-compression/

	//string str = "aabcccccaaa";
	//string str = "aabbcc";
	string str = "aaaaagooooogllletesttttttttswsssssssssssss";

	string ret = compress(str);
	cout << ret << endl;

	return 0;
}

