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
	 * 获得该 Profile 内容的 IniValue::StrMap
	 * 返回 IniValue::StrMap
	 * StrMap 中 username 对应 Profile.username，password 对应 Profile.password
	 */
	IniValue::StrMap GetProfileMap()
	{
		IniValue::StrMap profileMap;
		profileMap["username"] = username;
		profileMap["password"] = password;

		return profileMap;
	}

	// 用户名
	string username;
	// 密码
	string password;
};

bool operator< (const Profile& a, const Profile& b);

class ProfileManager
{
public:
	typedef map<string, Profile> ProfileMap;

	/*
	 * 工厂方法
	 * 接受 ini 文件路径或 IniFileProcessor 作为构造参数
	 * 新建一个 ProfileManager，并返回其指针
	 * 用户需要自行删除该指针指向的内存空间！！！
	 */
	static ProfileManager* GetProfileManager(const tstring& iniFilePath);
	static ProfileManager* GetProfileManager(const IniFileProcessor& processor);

	// 获得当前 presetting 的名称
	string GetPresettingName() const;
	// 设置 presetting 的名称
	void SetPresetting(const string& presetting);
	/* 
	 * 获得设置为 presetting 的 Profile
	 * 如果没有对应 presetting 的 Profile，则返回一个默认构造的 Profile
	 * 不会导致 settings 的变化
	 */
	Profile GetPresetting();

	// 获得当前 device 值
	string GetDevice() const;
	// 设置 device 值
	void SetDevice(const string& device);
	// 获得当前 real_ip 值
	bool GetRealIp() const;
	// 设置当前 real_ip 值
	void SetRealIp(const bool realIp);

	// 获得当前的所有 Profile
	ProfileMap GetProfiles() const;
	/*
	 * 刷新
	 * 会重新读取文件，当前未写入文件的修改都将被丢弃
	 */
	void Refresh();
	// 添加或修改指定的 Profile
	void AddOrModify(const Profile& profile);
	// 删除指定名称的 Profile
	void Delete(const string& profileName);

	// 保存文件，刷新数据
	void Save();

private:
	/* 
	 * 构造函数
	 * 必须使用工厂方法新建 ProfileManager
	 */
	ProfileManager(const tstring& iniFilePath);
	ProfileManager(const IniFileProcessor& processor);

	// 读取 ini 文件数据
	void Init();
	// 填充 Profiles
	void FillProfiles();

	IniFileProcessor iniProcessor;
	IniFileProcessor::IniMap settings;
	ProfileMap profiles;
	string presetting;
	string device;
	bool realIp;
};

#endif
