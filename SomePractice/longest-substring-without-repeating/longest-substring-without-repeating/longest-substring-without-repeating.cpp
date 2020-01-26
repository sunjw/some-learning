// longest-substring-without-repeating.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

static int lengthOfLongestSubstring(const string& s)
{
	size_t lastNoRepeatLen = 0;
	size_t maxNoRepeatLen = lastNoRepeatLen;

	map<char, size_t> lastOccur;
	size_t sLen = s.length();
	for (size_t i = 0; i < sLen; ++i)
	{
		char ch = s[i];
		map<char, size_t>::iterator itr = lastOccur.find(ch);
		size_t noRepeatLen = 0;
		if (itr == lastOccur.end())
		{
			// Never found.
			noRepeatLen = i + 1;
		}
		else
		{
			noRepeatLen = i - itr->second;
		}

		if (noRepeatLen > lastNoRepeatLen)
			noRepeatLen = lastNoRepeatLen + 1;

		if (noRepeatLen > maxNoRepeatLen)
			maxNoRepeatLen = noRepeatLen;

		lastNoRepeatLen = noRepeatLen;

		lastOccur[ch] = i;
	}

	return maxNoRepeatLen;
}

int main()
{
	// https://www.lintcode.com/problem/longest-substring-without-repeating-characters/

	//string s = "abcabcbb";
	//string s = "bbbbb";
	string s = "an++--viaj";
	//string s = "ejtdfngsdnnkgpkvtigsq";

	int maxLen = lengthOfLongestSubstring(s);
	cout << maxLen;

	return 0;
}

