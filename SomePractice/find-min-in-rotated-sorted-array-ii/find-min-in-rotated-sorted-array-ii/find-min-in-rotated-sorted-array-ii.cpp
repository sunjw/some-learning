// find-min-in-rotated-sorted-array-ii.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include <vector>

using namespace std;

static int recFindMin(vector<int>& nums, size_t startIdx, size_t endIdx)
{
	int min = 0;

	if (startIdx == endIdx)
	{
		min = nums[startIdx];
		return min;
	}

	size_t midIdx = (startIdx + endIdx) / 2;

	int startVal = nums[startIdx];
	int midVal = nums[midIdx];
	int endVal = nums[endIdx];

	if (startVal > midVal)
	{
		startIdx = startIdx;
		endIdx = midIdx;
		return recFindMin(nums, startIdx, endIdx);
	}
	else if (midVal > endVal)
	{
		startIdx = midIdx + 1;
		endIdx = endIdx;
		return recFindMin(nums, startIdx, endIdx);
	}
	else
	{
		min = recFindMin(nums, startIdx, midIdx);
		int min2 = recFindMin(nums, midIdx + 1, endIdx);
		if (min2 < min)
			min = min2;
		return min;
	}

	return min;
}

int findMin(vector<int>& nums)
{
	int min = 0;
	size_t numsLen = nums.size();
	size_t startIdx = 0;
	size_t endIdx = numsLen - 1;

	if (numsLen == 0)
		return min;

	min = recFindMin(nums, startIdx, endIdx);
	return min;
}

int main()
{
	// https://www.lintcode.com/problem/find-minimum-in-rotated-sorted-array-ii/

	vector<int> nums;

	//nums = { 1 };
	//nums = { 2, 1 };
	nums = { 4, 5, 6, 7, 0, 1, 2 };
	//nums = { 1, 1, -1, 1 };

	int min = findMin(nums);

	cout << min;

	return 0;
}

