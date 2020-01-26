// lowest-common-ancestor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>

using namespace std;

class TreeNode {
public:
	int val;
	TreeNode *left, *right;
	TreeNode(int val) 
	{
		this->val = val;
		this->left = this->right = NULL;
	}
};

class TreeNodeEx : public TreeNode
{
public:
	TreeNodeEx(int val) : TreeNode(val)
	{ }

	static TreeNodeEx *createTreeNode(int val, TreeNodeEx *left = NULL, TreeNodeEx *right = NULL)
	{
		TreeNodeEx *node = new TreeNodeEx(val);
		node->left = left;
		node->right = right;
		return node;
	}

	static void destroyTree(TreeNode *root)
	{
		if (!root)
			return;

		TreeNodeEx::destroyTree(root->left);
		TreeNodeEx::destroyTree(root->right);

		delete root;
	}

	void insertLeftNode(TreeNodeEx *node)
	{
		left = node;
	}

	void insertRightNode(TreeNodeEx *node)
	{
		right = node;
	}

	TreeNodeEx *insertLeftVal(int val)
	{
		TreeNodeEx *node = TreeNodeEx::createTreeNode(val);
		insertLeftNode(node);
		return node;
	}

	TreeNodeEx *insertRightVal(int val)
	{
		TreeNodeEx *node = TreeNodeEx::createTreeNode(val);
		insertRightNode(node);
		return node;
	}
};

static int findNode(TreeNode *from, TreeNode *A, TreeNode *B, TreeNode **lca)
{
	if (from == NULL)
		return 0;
	
	int ret = 0;
	if (from == A || from == B)
		ret = 1;

	int left = findNode(from->left, A, B, lca);
	int right = findNode(from->right, A, B, lca);

	if (*lca == NULL)
	{
		if (ret == 0 && left == 1 && right == 1)
		{
			*lca = from;
		}
		if (ret == 1 && (left == 1 || right == 1))
		{
			*lca = from;
		}
	}

	ret = ret + left + right;
	return ret;
}

TreeNode *lowestCommonAncestor(TreeNode *root, TreeNode *A, TreeNode *B) {
	TreeNode *ret = NULL;

	if (!root)
		return NULL;

	if (A == B)
		return A;

	findNode(root, A, B, &ret);

	return ret;
}


int main()
{
	// https://www.lintcode.com/problem/lowest-common-ancestor-of-a-binary-tree/

	//TreeNodeEx *root = TreeNodeEx::createTreeNode(4);
	//TreeNodeEx *a = NULL, *b = NULL;

	//TreeNodeEx *itr = NULL;
	//itr = root->insertLeftVal(3);
	//a = itr;
	//itr = root->insertRightVal(7);
	//b = itr->insertLeftVal(5);
	//itr->insertRightVal(6);

	TreeNodeEx *root = TreeNodeEx::createTreeNode(1);
	TreeNodeEx *a = root, *b = root;

	TreeNode *lca = lowestCommonAncestor(root, a, b);

	TreeNodeEx::destroyTree(root);
    
	return 0;
}

