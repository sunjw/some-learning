// merge-k-sorted-arrays.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include <vector>
#include <map>
#include <queue>

using namespace std;

vector<int> mergekSortedArrays(vector< vector<int> >& arrays) 
{
	vector<int> merged;

	map<int, queue<size_t> > minValMap;

	size_t arrayCount = arrays.size();
	vector<size_t> arrayPos(arrayCount);
	for (size_t i = 0; i < arrayCount; ++i)
	{
		vector<int>& arrayItr = arrays[i];
		arrayPos[i] = 0;
		if (arrayItr.size() > 0)
		{
			int val = arrayItr[0];
			minValMap[val].push(i);
		}
	}

	while (1)
	{
		map<int, queue<size_t> >::iterator minItr = minValMap.begin();
		if (minItr == minValMap.end())
			break;

		int minVal = minItr->first;
		merged.push_back(minVal);
		queue<size_t>& minQueue = minItr->second;
		size_t arrayIdx = minQueue.front();
		minQueue.pop();
		if (minQueue.empty())
		{
			minValMap.erase(minItr);
		}
		size_t arrayIdxPos = arrayPos[arrayIdx];
		arrayIdxPos++;
		vector<int>& arrayItr = arrays[arrayIdx];
		if (arrayIdxPos < arrayItr.size())
		{
			int arrayIdxVal = arrayItr[arrayIdxPos];
			minValMap[arrayIdxVal].push(arrayIdx);
			arrayPos[arrayIdx] = arrayIdxPos;
		}
	}

	return merged;
}

int main()
{
	vector< vector<int> > arrays;

	//arrays.push_back({ 1, 3, 5, 7 });
	//arrays.push_back({ 2, 4, 6 });
	//arrays.push_back({ 0, 8, 9, 10, 11 });

	arrays.push_back({ 1, 2, 3 });
	arrays.push_back({ 1, 2 });

	vector<int> merged = mergekSortedArrays(arrays);

	return 0;
}

