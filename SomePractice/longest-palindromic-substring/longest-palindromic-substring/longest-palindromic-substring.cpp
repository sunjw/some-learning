// longest-palindromic-substring.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include <string>

using namespace std;

static void searchPalindrome(const string& s, size_t sLen, size_t& palinStart, size_t& palinEnd)
{
	while (1)
	{
		if (palinStart == 0 || palinEnd == sLen - 1)
			break;

		char prev = s[palinStart - 1];
		char next = s[palinEnd + 1];
		if (prev == next)
		{
			palinStart--;
			palinEnd++;
		}
		else
			break;
	}
}

string longestPalindrome(string& s) 
{
	string longest;
	size_t longestLen = 0;

	size_t sLen = s.length();
	for (size_t i = 0; i < sLen; ++i)
	{
		char ch = s[i];
		
		size_t palinStart = i;
		size_t palinEnd = i;
		searchPalindrome(s, sLen, palinStart, palinEnd);
		size_t palinLen = palinEnd - palinStart + 1;
		if (palinLen > longestLen)
		{
			longest = s.substr(palinStart, palinLen);
			longestLen = palinLen;
		}
		
		if (i > 0)
		{
			char prev = s[i - 1];
			if (prev == ch)
			{
				palinStart = i - 1;
				palinEnd = i;
				searchPalindrome(s, sLen, palinStart, palinEnd);
				palinLen = palinEnd - palinStart + 1;
				if (palinLen > longestLen)
				{
					longest = s.substr(palinStart, palinLen);
					longestLen = palinLen;
				}
			}
		}
	}

	return longest;
}

int main()
{
	// https://www.lintcode.com/problem/longest-palindromic-substring/

	//string s = "abcdzdcab";
	//string s = "aba";
	//string s = "bbzazzzz";
	string s = "";

	string palindrome = longestPalindrome(s);
	cout << palindrome << endl;

	return 0;
}
