/*
 * runner.cpp
 *
 *  Created on: 2010-9-25
 *      Author: sun
 */
#include <iostream>
#include <cstdlib>
#include <string>

#include "global.h"
#include "HuaweiNetwork.h"
#include "ProfileManager.h"
#include "App.h"

using namespace std;

void join(pthread_t* threads)
{
	join_thread(threads[0]);
	join_thread(threads[1]);
}

void debug_args(const thread_arg& arg)
{
	cout << arg.nic << endl;
	cout << arg.name << endl;
	cout << arg.pswd << endl;
	cout << *arg.is_connect << endl;
}

int main()
{
	string cmd;
	bool connect_flag = true, disconnect_flag = false;

	pthread_t threads[2] =
	{ 0 };

	ProfileManager* pProfileMan = ProfileManager::GetProfileManager("h3c.ini");

	thread_arg arg[2];
	Profile profile = pProfileMan->GetPresetting();
	arg[0].nic = pProfileMan->GetDevice();
	arg[0].real_ip = pProfileMan->GetRealIp();
	arg[0].name = profile.username;
	arg[0].pswd = profile.password;
	arg[1] = arg[0];
	arg[0].is_connect = &connect_flag;
	arg[1].is_connect = &disconnect_flag;

	//debug_args(arg[0]);
	//debug_args(arg[1]);

	while (1)
	{
		cout << "Select a command:" << endl;
		cout << "1: Connect" << endl;
		cout << "2: Disconnect" << endl;
		cout << "3: Quit" << endl;
		cin >> cmd;

		if (cmd.compare("1") == 0)
		{
			// Connect
			threads[0] = work(&arg[0]);
		}
		else if (cmd.compare("2") == 0)
		{
			// Disconnect
			threads[1] = work(&arg[1]);
		}
		else if (cmd.compare("3") == 0)
		{
			threads[1] = work(&arg[1]);
			break;
		}
		else
		{
			cout << "Unknown command." << endl;
		}
	}

	join(threads);

	delete pProfileMan;

	return 0;
}
