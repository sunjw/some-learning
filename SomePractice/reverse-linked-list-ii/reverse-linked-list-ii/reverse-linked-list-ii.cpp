// reverse-linked-list-ii.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>

class ListNode
{
public:
	int val;
	ListNode *next;
	ListNode(int val) 
	{
		this->val = val;
		this->next = NULL;
	}
};

class ListNodeEx : public ListNode
{
public:
	ListNodeEx(int val): ListNode(val)
	{}

	static ListNodeEx *createListNode(int val)
	{
		ListNodeEx *node = new ListNodeEx(val);
		return node;
	}

	static void destroyList(ListNode *first)
	{
		ListNode *itr = first;
		while (1)
		{
			if (!itr)
				break;

			ListNode *next = itr->next;
			delete itr;
			itr = next;
		}
	}

	ListNodeEx *append(int val)
	{
		ListNodeEx *node = ListNodeEx::createListNode(val);
		next = node;
		return node;
	}
};

ListNode *reverseBetween(ListNode *head, int m, int n) 
{
	int count = 1;
	ListNode *mPrev = NULL; // NULLable
	ListNode *mNode = NULL;
	ListNode *itr = head;
	ListNode *itrPrev = NULL;
	while (1)
	{
		ListNode *itrNext = itr->next;
		ListNode *itrPrevSave = itr;
		
		if (count == m - 1)
			mPrev = itr;

		if (count == m)
			mNode = itr;

		if (count > m && count <= n)
		{
			itrPrev->next = itrNext;
			itrPrevSave = itrPrev;
			itr->next = mNode;
			mNode = itr;
			if (mPrev)
				mPrev->next = itr;
		}

		itrPrev = itrPrevSave;
		itr = itrNext;
		++count;

		if (count > n || !itr)
			break;
	}

	if (!mPrev)
		return mNode;
	return head;
}

int main()
{
	// https://www.lintcode.com/problem/reverse-linked-list-ii/

	//ListNodeEx *first = ListNodeEx::createListNode(1);
	//ListNodeEx *itr = first;
	//itr = itr->append(2);
	//itr = itr->append(3);
	//itr = itr->append(4);
	//itr = itr->append(5);
	//int m = 2, n = 4;

	//ListNodeEx *first = ListNodeEx::createListNode(1);
	//ListNodeEx *itr = first;
	//itr = itr->append(2);
	//itr = itr->append(3);
	//itr = itr->append(4);
	//int m = 2, n = 3;

	ListNodeEx *first = ListNodeEx::createListNode(1);
	ListNodeEx *itr = first;
	itr = itr->append(2);
	itr = itr->append(3);
	itr = itr->append(4);
	int m = 1, n = 4;

	first = (ListNodeEx *)reverseBetween(first, m, n);

	ListNodeEx::destroyList(first);

	return 0;
}

