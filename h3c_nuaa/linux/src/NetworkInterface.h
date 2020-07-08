/*
 * NetworkInterface.h
 *
 *  Created on: 2010-10-29
 *      Author: sun
 */

#ifndef NETWORKINTERFACE_H_
#define NETWORKINTERFACE_H_
#include <string>

#include "global.h"

long GetTickCount();
void GetInterfaceMac(const std::string& interface, int *mac);
std::string GetInterfaceIP(const std::string& interface);
int GetSystemInterfaceInfo(InterfaceInfo * infoarray);

#endif /* NETWORKINTERFACE_H_ */
