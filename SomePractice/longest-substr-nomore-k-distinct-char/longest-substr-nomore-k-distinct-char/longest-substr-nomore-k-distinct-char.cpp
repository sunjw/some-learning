// longest-substr-nomore-k-distinct-char.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include <string>
#include <queue>
#include <map>

using namespace std;

int lengthOfLongestSubstringKDistinct(string& s, int k)
{
	if (k == 0)
		return 0;

	int maxSubstrLen = 0;
	queue<char> curSubstring;
	int curSubstrLen = 0;
	map<char, size_t> curCharCount;
	size_t curDistinctCount = 0;

	size_t sLen = s.length();
	for (size_t i = 0; i < sLen; ++i)
	{
		char ch = s[i];
		map<char, size_t>::iterator itr = curCharCount.find(ch);
		if (itr == curCharCount.end())
		{
			// New distinct char
			while (curDistinctCount >= k)
			{
				// Need pop front
				char substrFront = curSubstring.front();
				curSubstring.pop();
				curSubstrLen--;
				map<char, size_t>::iterator itrFront = curCharCount.find(substrFront);
				itrFront->second--;
				if (itrFront->second == 0)
				{
					// Remove front char
					curCharCount.erase(itrFront);
					--curDistinctCount;
				}
			}

			// Add it
			curSubstring.push(ch);
			curSubstrLen++;
			curCharCount[ch] = 1;
			curDistinctCount++;
		}
		else
		{
			// Exists char
			curSubstring.push(ch);
			curSubstrLen++;
			curCharCount[ch]++;
		}

		if (curSubstrLen > maxSubstrLen)
			maxSubstrLen = curSubstrLen;
	}

	return maxSubstrLen;
}

int main()
{
	// https://www.lintcode.com/problem/longest-substring-with-at-most-k-distinct-characters/

	//string s = "eceba";
	//int k = 3;
	//string s = "WORLD";
	//int k = 4;
	string s = "W";
	int k = 0;

	int len = lengthOfLongestSubstringKDistinct(s, k);
	cout << len;

	return 0;
}

