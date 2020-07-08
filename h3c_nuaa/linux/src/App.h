/*
 * App.h
 *
 *  Created on: 2010-10-3
 *      Author: sun
 */

#ifndef APP_H_
#define APP_H_
#include <string>
#include <pthread.h>

#include "global.h"

struct thread_arg
{
	std::string name;
	std::string pswd;
	std::string nic;
	bool real_ip;
	bool* is_connect;
};

void dispaly_if(InterfaceInfo* pIfInfo, int ifNum);
void display_mac(int* mac);

void* working_thread(void* argu);
void join_thread(pthread_t thread);

pthread_t work(thread_arg* arg);

#endif /* APP_H_ */
