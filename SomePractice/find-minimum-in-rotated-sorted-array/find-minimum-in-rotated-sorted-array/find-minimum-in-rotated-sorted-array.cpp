// find-minimum-in-rotated-sorted-array.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

static int findMin1(vector<int>& nums)
{
	vector<int>::iterator itr = nums.begin();
	int prevNum = *itr;
	int min = prevNum;
	for (; itr != nums.end(); ++itr)
	{
		int curNum = *itr;
		if (curNum < prevNum)
		{
			min = curNum;
			break;
		}
	}

	return min;
}

static int findMin2(vector<int>& nums)
{
	int min = 0;
	size_t numsLen = nums.size();
	size_t startIdx = 0;
	size_t endIdx = numsLen - 1;

	if (numsLen == 0)
		return min;
	
	while (1)
	{
		if (startIdx == endIdx)
		{
			min = nums[startIdx];
			break;
		}

		size_t midIdx = (startIdx + endIdx) / 2;
		
		int startVal = nums[startIdx];
		int midVal = nums[midIdx];
		int endVal = nums[endIdx];

		if (startVal > midVal)
		{
			startIdx = startIdx;
			endIdx = midIdx;
			continue;
		}
		else if (midVal > endVal)
		{
			startIdx = midIdx + 1;
			endIdx = endIdx;
			continue;
		}
		else
		{
			min = startVal;
			break;
		}
	}

	return min;
}

int main()
{
	// https://www.lintcode.com/problem/find-minimum-in-rotated-sorted-array/

	string input;
	vector<int> nums;

	nums = { 1 };
	//nums = { 2, 1 };
	//nums = { 4, 5, 6, 7, 0, 1, 2 };

	//int min = findMin1(nums);
	int min = findMin2(nums);

	cout << min;

	return 0;
}

