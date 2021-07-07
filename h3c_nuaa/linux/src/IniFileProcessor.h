/*
 * IniFileProcessor class header file
 * Author: Sun Junwen
 * Version: 1.1
 */
#ifndef _INI_FILE_PROCESSOR_H_
#define _INI_FILE_PROCESSOR_H_

#include <cstdlib>
#include <string>
#include <map>

#include "IniValue.h"

using namespace std;

class IniFileProcessor
{
public:
	typedef map<string, IniValue> IniMap;

	/*
	 * Constructor
	 * Specific the ini file name
	 */
	IniFileProcessor(string fileName)
		:strFileName(fileName)
	{}

	// Get info from file
	inline IniMap GetInfo(bool bSection)
	{ return GetInfo(bSection, true); }
	IniMap GetInfo(bool bProcSection, bool bRefresh);

	// Set value into map
	void SetMap(const IniMap& map)
	{ iniMap = map; }

	// Save value into file
	void Save();

	// Convert value to string
	inline string ToString() const
	{ return ToString(iniMap); }
	string ToString(IniMap map) const;

private:
	string strFileName;
	IniMap iniMap;

	// Add a section to map
	void AddSection(const string& sectionName, const IniValue::StrMap& sectionMap);
};

#endif
