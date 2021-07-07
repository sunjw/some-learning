#ifndef _PROFILE_MANAGER_H_
#define _PROFILE_MANAGER_H_

#include <string>
#include <map>

#include "strhelper.h"
#include "IniFileProcessor.h"

using namespace std;
using namespace sunjwbase;

class Profile
{
public:
	Profile(string username = "", string password = "")
		:username(username), password(password)
	{ }

	/*
	 * ��ø� Profile ���ݵ� IniValue::StrMap
	 * ���� IniValue::StrMap
	 * StrMap �� username ��Ӧ Profile.username��password ��Ӧ Profile.password
	 */
	IniValue::StrMap GetProfileMap()
	{
		IniValue::StrMap profileMap;
		profileMap["username"] = username;
		profileMap["password"] = password;

		return profileMap;
	}

	// �û���
	string username;
	// ����
	string password;
};

bool operator< (const Profile& a, const Profile& b);

class ProfileManager
{
public:
	typedef map<string, Profile> ProfileMap;

	/*
	 * ��������
	 * ���� ini �ļ�·���� IniFileProcessor ��Ϊ�������
	 * �½�һ�� ProfileManager����������ָ��
	 * �û���Ҫ����ɾ����ָ��ָ����ڴ�ռ䣡����
	 */
	static ProfileManager* GetProfileManager(const tstring& iniFilePath);
	static ProfileManager* GetProfileManager(const IniFileProcessor& processor);

	// ��õ�ǰ presetting ������
	string GetPresettingName() const;
	// ���� presetting ������
	void SetPresetting(const string& presetting);
	/* 
	 * �������Ϊ presetting �� Profile
	 * ���û�ж�Ӧ presetting �� Profile���򷵻�һ��Ĭ�Ϲ���� Profile
	 * ���ᵼ�� settings �ı仯
	 */
	Profile GetPresetting();

	// ��õ�ǰ device ֵ
	string GetDevice() const;
	// ���� device ֵ
	void SetDevice(const string& device);
	// ��õ�ǰ real_ip ֵ
	bool GetRealIp() const;
	// ���õ�ǰ real_ip ֵ
	void SetRealIp(const bool realIp);

	// ��õ�ǰ������ Profile
	ProfileMap GetProfiles() const;
	/*
	 * ˢ��
	 * �����¶�ȡ�ļ�����ǰδд���ļ����޸Ķ���������
	 */
	void Refresh();
	// ��ӻ��޸�ָ���� Profile
	void AddOrModify(const Profile& profile);
	// ɾ��ָ�����Ƶ� Profile
	void Delete(const string& profileName);

	// �����ļ���ˢ������
	void Save();

private:
	/* 
	 * ���캯��
	 * ����ʹ�ù��������½� ProfileManager
	 */
	ProfileManager(const tstring& iniFilePath);
	ProfileManager(const IniFileProcessor& processor);

	// ��ȡ ini �ļ�����
	void Init();
	// ��� Profiles
	void FillProfiles();

	IniFileProcessor iniProcessor;
	IniFileProcessor::IniMap settings;
	ProfileMap profiles;
	string presetting;
	string device;
	bool realIp;
};

#endif
