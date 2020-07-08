/*
 * NetworkInterface.cpp
 *
 *  Created on: 2010-10-29
 *      Author: sun
 */
#include <string>
#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <arpa/inet.h>

#include "global.h"
#include "NetworkInterface.h"

using namespace std;

long GetTickCount()
{
	tms tm;
	return times(&tm);
}

// return the MAC of the interface, in the int array parameter
void GetInterfaceMac(const string& interface, int *mac)
{
	int fd;
	struct ifreq ifr;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("arp socket error");
		return;//exit(-1);
	}
	//strcpy(ifr.ifr_name, "eth0");
	strcpy(ifr.ifr_name, interface.c_str());
	ifr.ifr_addr.sa_family = AF_INET;

	if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
	{
		perror("ioctl SIOCGIFHWADDR error");
		return;//exit(-1);
	}
	for (int i = 0; i < 6; i++)
	{
		mac[i] = (unsigned char) ifr.ifr_ifru.ifru_hwaddr.sa_data[i];
	}
}

string GetInterfaceIP(const string& interface)
{
	int fd;
	struct ifreq ifr;
	string ipaddr("");

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("arp socket error");
		return string();//exit(-1);
	}
	//strcpy(ifr.ifr_name, "eth0");
	strcpy(ifr.ifr_name, interface.c_str());
	ifr.ifr_addr.sa_family = AF_INET;

	if (!(ioctl(fd, SIOCGIFADDR, &ifr)))
	{
		ipaddr = inet_ntoa(((struct sockaddr_in*) (&ifr.ifr_addr))->sin_addr);

	}

	return ipaddr;
}

int GetSystemInterfaceInfo(InterfaceInfo* infoarray)
{
	int fd;
	struct ifreq buf[MAXINTERFACES];
	struct ifconf ifc;

	int interfacenum;
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
	{
		ifc.ifc_len = sizeof buf;
		ifc.ifc_buf = (caddr_t) buf;
		if (!ioctl(fd, SIOCGIFCONF, (char *) &ifc))
		{
			interfacenum = ifc.ifc_len / sizeof(struct ifreq);

			if (infoarray != NULL)
			{
				for (int i = 0; i < interfacenum; i++)
				{
					infoarray[i].name = buf[i].ifr_name;

					//  Get IP of the net card
					if (!(ioctl(fd, SIOCGIFADDR, (char *) &buf[i])))
					{
						infoarray[i].ipaddr
								= inet_ntoa(
										((struct sockaddr_in*) (&buf[i].ifr_addr))->sin_addr);

					}
					else
					{
						infoarray[i].ipaddr = "";
					}

					//Get HW ADDRESS of the net card
					if (!(ioctl(fd, SIOCGIFHWADDR, (char *) &buf[i])))
					{
						for (int j = 0; j < 6; j++)
						{
							infoarray[i].macaddr[j]
									= (unsigned char) buf[i].ifr_hwaddr.sa_data[j];
						}

					}
					else
					{
						infoarray[i].macaddr[0] = -1;
					}
				}
			}

		}
		else
		{
			interfacenum = -1;
		}
	}
	else
	{
		interfacenum = -2;
	}

	close(fd);

	return interfacenum;
}
