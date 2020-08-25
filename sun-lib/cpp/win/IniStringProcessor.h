/*
 * IniStringProcessor class header file
 * Author: Sun Junwen
 * Version: 1.0
 */
#ifndef _INI_STRING_PROCESSOR_H_
#define _INI_STRING_PROCESSOR_H_

#include "strhelper.h"
#include "IniProcessor.h"

using namespace std;
using namespace sunjwbase;

class IniStringProcessor: public IniProcessor
{
public:
	/*
	 * Constructor
	 * Specific the ini file name
	 */
	IniStringProcessor(string input)
	{
		m_iniStr = input;
	}

	virtual void Save();

	inline IniMap GetInfo(bool bSection)
	{ return GetInfo(bSection, true); }

	IniProcessor::IniMap GetInfo(bool bProcSection, bool bRefresh);

	string GetIniStr() const
	{ return m_iniStr; }

private:
	string m_iniStr;
};

#endif
