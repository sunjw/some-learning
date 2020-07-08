#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>

#include "ProfileManager.h"

using namespace std;

bool operator< (const Profile& a, const Profile& b)
{
	return a.username < b.username;
}

ProfileManager::ProfileManager(const string& iniFilePath)
:iniProcessor(IniFileProcessor(iniFilePath))
{
	Init();
}

ProfileManager::ProfileManager(const IniFileProcessor& processor)
:iniProcessor(processor)
{
	Init();
}

ProfileManager* ProfileManager::GetProfileManager(const string& iniFilePath)
{
	return new ProfileManager(iniFilePath);
}

ProfileManager* ProfileManager::GetProfileManager(const IniFileProcessor& processor)
{
	return new ProfileManager(processor);
}

void ProfileManager::Init()
{
	settings = iniProcessor.GetInfo(true);
	presetting = settings["presetting"].GetStrValue();
	device = settings["device"].GetStrValue();
	realIp = (settings["real_ip"].GetStrValue().compare("1") == 0) ? true
				: false;

	FillProfiles();
}

void ProfileManager::FillProfiles()
{
	// clean profiles
	profiles.clear();
	
	IniFileProcessor::IniMap::iterator itr = settings.begin();
	for(; itr != settings.end(); ++itr)
	{
		//string key = (*itr).first;
		IniValue value = (*itr).second;

		if(!value.IsStrValue())
		{
			IniValue::StrMap setting = value.GetMapValue();

			string username = setting["username"];
			string password = setting["password"];

			profiles[username] = Profile(username, password);
		}
	}
}

void ProfileManager::Save()
{
	IniFileProcessor::IniMap newSettings;
	newSettings["presetting"] = IniValue(presetting);
	newSettings["device"] = IniValue(device);

	ProfileManager::ProfileMap::iterator itr = profiles.begin();
	for(; itr != profiles.end(); ++itr)
	{
		string key = (*itr).first;
		IniValue::StrMap value = (*itr).second.GetProfileMap();

		IniValue iniValue(false);
		iniValue.SetMapValue(value);

		newSettings[key] = iniValue;
	}

	settings = newSettings;
	iniProcessor.SetMap(settings);

	iniProcessor.Save();

	Refresh();
}

void ProfileManager::Refresh()
{
	Init();
}

string ProfileManager::GetPresettingName() const
{
	return presetting;
}

/*
 * 没有的话不能直接 operator[]
 */
Profile ProfileManager::GetPresetting()
{
	if(profiles.find(presetting) != profiles.end())
		return profiles[presetting];
	else
		return Profile();
}

void ProfileManager::SetPresetting(const string& presetting)
{
	this->presetting = presetting;
}

string ProfileManager::GetDevice() const
{
	return device;
}

void ProfileManager::SetDevice(const string& device)
{
	this->device = device;
}

bool ProfileManager::GetRealIp() const
{
	return realIp;
}

void ProfileManager::SetRealIp(const bool realIp)
{
	this->realIp = realIp;
}

ProfileManager::ProfileMap ProfileManager::GetProfiles() const
{
	return profiles;
}

void ProfileManager::AddOrModify(const Profile& profile)
{
	profiles[profile.username] = profile;

	if(!profiles.empty() && GetPresetting().username.compare("") == 0)
		presetting = (*(profiles.begin())).first;
}

void ProfileManager::Delete(const string& profileName)
{
	profiles.erase(profileName);

	if(profileName.compare(presetting) == 0 && !profiles.empty())
		presetting = (*(profiles.begin())).first;
	else if(profiles.empty())
		presetting = "";
}
