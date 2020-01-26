// find-leaves-of-binary-tree.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include <vector>

using namespace std;

class TreeNode 
{
public:
	int val;
	TreeNode *left, *right;

	TreeNode(int val) 
	{
		this->val = val;
		this->left = this->right = NULL;
	}
};

class TreeNodeEx: public TreeNode
{
public:
	TreeNodeEx(int val): TreeNode(val)
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

static void travelTreeToCollectLeaves(TreeNode *node, vector< vector<int> > *leafNodes, int *curLevel)
{
	int realLevel = 0;
	int childLevel = 0;
	
	if (node->left)
	{
		travelTreeToCollectLeaves(node->left, leafNodes, &childLevel);
		int leftLevel = childLevel + 1;
		if (leftLevel > realLevel)
			realLevel = leftLevel;
	}
	if (node->right)
	{
		travelTreeToCollectLeaves(node->right, leafNodes, &childLevel);
		int rightLevel = childLevel + 1;
		if (rightLevel > realLevel)
			realLevel = rightLevel;
	}

	*curLevel = realLevel;
	if (leafNodes->size() <= realLevel)
		leafNodes->resize(realLevel + 1);
	(*leafNodes)[realLevel].push_back(node->val);
}

vector< vector<int> > findLeaves(TreeNode *root)
{
	vector< vector<int> > leafNodes;

	if (root)
	{
		int level = 0;
		travelTreeToCollectLeaves(root, &leafNodes, &level);
	}

	return leafNodes;
}

int main()
{
	// https://www.lintcode.com/problem/find-leaves-of-binary-tree/

	//TreeNodeEx *root = TreeNodeEx::createTreeNode(1);
	//TreeNodeEx *itr = NULL;
	//itr = root->insertLeftVal(2);
	//itr->insertLeftVal(4);
	//itr->insertRightVal(5);
	//itr = root->insertRightVal(3);

	//TreeNodeEx *root = TreeNodeEx::createTreeNode(1);
	//TreeNodeEx *itr = NULL;
	//itr = root->insertLeftVal(2);
	//itr->insertLeftVal(4);
	//itr = itr->insertRightVal(5);
	//itr->insertRightVal(6);
	//itr = root->insertRightVal(3);

	TreeNodeEx *root = TreeNodeEx::createTreeNode(1);
	TreeNodeEx *itr = NULL;
	itr = root->insertLeftVal(2);
	itr->insertLeftVal(4);
	itr = root->insertRightVal(3);

	vector< vector<int> > leaves = findLeaves(root);

	TreeNodeEx::destroyTree(root);

	return 0;
}

