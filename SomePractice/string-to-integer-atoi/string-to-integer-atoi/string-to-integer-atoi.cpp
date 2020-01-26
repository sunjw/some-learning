// string-to-integer-atoi.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <stdint.h>
#include <string>
#include <iostream>

using namespace std;

int main()
{
	// https://www.lintcode.com/problem/string-to-integer-atoi/

	string input;
	int64_t iVal = 0;
	int sign = 1;

	cin >> input;
	
	size_t inputLen = input.length();
	for (size_t i = 0; i < inputLen; ++i)
	{
		char ch = input[i];
		
		if (ch < '0' || ch > '9')
		{
			if (i == 0 && (ch == '+' || ch == '-'))
			{
				if (ch == '-')
					sign = -1;
				continue;
			}
			break;
		}

		int chVal = ch - '0';
		iVal = iVal * 10 + chVal;
		if (sign == 1 && iVal > INT_MAX)
		{
			iVal = INT_MAX;
			break;
		}
		if (sign == -1 && (sign * iVal) < INT_MIN)
		{
			iVal = INT_MIN;
			break;
		}
	}

	iVal = iVal * sign;

	cout << iVal << endl;

	return 0;
}

