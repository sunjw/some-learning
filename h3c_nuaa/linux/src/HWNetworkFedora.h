/*
 * HWNetworkFedora.h
 *
 *  Created on: 2010-10-29
 *      Author: sun
 */

#ifndef _HW_NETWORKFEDORA_H_
#define _HW_NETWORKFEDORA_H_
#include <string>
#include <iostream>

#include "HuaweiNetwork.h"

using namespace std;

class HuaweiNetworkFedora: public HuaweiNetwork
{
public:
	HuaweiNetworkFedora(ostream& out, const string& username,
			const string& passwd, const string& interfacename, int* ip,
			int* mac, bool real_ip, const string& version = "2.40-0145");

private:
	void SetMd5Buf(PPASSWORDFRM pBuf, const u_char ID, const u_char *chap);
	void IPRenew(bool bRelease);
	void Message(unsigned int msg, const char* str);
	void Message(const char* str);

	bool SendUsername(const u_char Id);

	ostream& outStream;
	bool send_real_ip;

};

#endif /* _HW_NETWORKFEDORA_H_ */
