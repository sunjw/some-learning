/*
 * App.cpp
 *
 *  Created on: 2010-10-3
 *      Author: sun
 */
#include <iostream>
#include <cstdlib>
#include <string>
#include <pthread.h>

#include "global.h"
#include "App.h"
#include "HWNetworkFedora.h"
#include "NetworkInterface.h"

using namespace std;

void dispaly_if(InterfaceInfo* pIfInfo, int ifNum)
{
	for (int i = 0; i < ifNum; ++i)
	{
		cout << pIfInfo[i].name << endl;
		cout << pIfInfo[i].ipaddr << endl;
		display_mac(pIfInfo[i].macaddr);
		cout << endl;
	}
}

void display_mac(int* mac)
{
	for (int i = 0; i < 6; ++i)
	{
		cout << hex << mac[i];
		if (i < 5)
			cout << '.';
	}
}

void* working_thread(void* argu)
{
	thread_arg th_argu = *(thread_arg*) argu;
	bool is_connect = *th_argu.is_connect;

	int mac[6];
	int ip[4] =
	{ 192, 168, 0, 1 };

	// get MAC
	GetInterfaceMac(th_argu.nic, mac);
	//display_mac(mac);

	HuaweiNetworkFedora hwObj(cout, th_argu.name, th_argu.pswd, th_argu.nic, ip, mac,
			th_argu.real_ip, string("2.40-0145"));

	if (is_connect)
	{
		hwObj.Connect();
	}
	else
	{
		hwObj.DisConnect();
	}

	return NULL;
}

void join_thread(pthread_t thread)
{
	if (thread)
	{
		cout << "等待线程" << endl;
		pthread_join(thread, NULL);
	}
}

pthread_t work(thread_arg* arg)
{
	pthread_t thread;
	int rc;
	void *(*task)(void*) = working_thread;

	rc = pthread_create(&thread, NULL, task, (void*) arg);

	return thread;
}

