/*
 * IniStringProcessor class implementation file
 * Author: Sun Junwen
 * Version: 1.0
 */
#include <cstdlib>
#include <string>
#include <sstream>

#include "IniStringProcessor.h"

using namespace std;

void IniStringProcessor::Save()
{
	m_iniStr = ToString();
}

IniProcessor::IniMap IniStringProcessor::GetInfo(bool bProcSection, bool bRefresh)
{
	istringstream m_in(m_iniStr);
	return IniProcessor::GetInfo(m_in, bProcSection, bRefresh);
}
