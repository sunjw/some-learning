// MergeText.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>

#include "strhelper.h"

using namespace std;
using namespace sunjwbase;

typedef vector<string> StringVec;
typedef set<string> StringSet;

static void ReadInToSet(istream& in, StringSet& strings)
{
	int count = 0;
	string line;
	while (getline(in, line))
	{
		line = strtrim(line);
		strings.insert(line);
		++count;
	}
	cout << "Read " << count << " lines." << endl;
}

static void WriteSetToOut(ostream& out, StringSet& strings)
{
	int count = 0;
	StringSet::iterator itr = strings.begin();
	for (; itr != strings.end(); ++itr)
	{
		out << *itr << "\n";
		++count;
	}
	
	cout << "Write " << count << " lines." << endl;
}

int main(int argc, char *argv[])
{
	StringVec inputPaths;
	string outputPath;

	enum ArgState { ARG, ARG_INPUT, ARG_OUTPUT };
	ArgState argState = ARG;

	for (int i = 1; i < argc; ++i)
	{
		string arg(argv[i]);
		
		if (arg == "-i")
		{
			argState = ARG_INPUT;
			continue;
		}
		if (arg == "-o")
		{
			argState = ARG_OUTPUT;
			continue;
		}

		if (argState == ARG_INPUT)
		{
			inputPaths.push_back(arg);
		}
		if (argState == ARG_OUTPUT)
		{
			outputPath = arg;
		}
	}

	if (inputPaths.size() == 0 || outputPath == "")
	{
		cout << "Usage: MergeText -i [file1] [file2] ... -o [outputfile]" << endl;
		return 0;
	}

	StringSet strings;

	StringVec::const_iterator itr = inputPaths.begin();
	for (; itr != inputPaths.end(); ++itr)
	{
		ifstream fin(itr->c_str());

		ReadInToSet(fin, strings);

		fin.close();
	}

	ofstream foutMerge(outputPath.c_str());
	WriteSetToOut(foutMerge, strings);
	foutMerge.close();

	return 0;
}

