//
//  Util.cpp
//  NTFSTypeFixer
//
//  Created by Sun Junwen on 13-7-5.
//  Copyright (c) 2013 Sun Junwen. All rights reserved.
//

#include "Util.h"

#include <string>
#include <CoreFoundation/CoreFoundation.h>

#define BUFF_SIZE 4096

using namespace std;

string CFStringRefToString(CFStringRef cfsrString)
{
    char szBuff[BUFF_SIZE] = {0};
    CFStringGetCString(cfsrString, szBuff, BUFF_SIZE,
                       kCFStringEncodingUTF8);
    
    string str(szBuff);
    
    return str;
}

CFStringRef StringToCFStringRef(const string& str)
{
    CFStringRef cfzrStr = CFStringCreateWithCString(kCFAllocatorDefault,
                                                    str.c_str(),
                                                    kCFStringEncodingASCII);
    
    return cfzrStr;
}

CFURLRef CFStringRefToCFURLRef(CFStringRef cfsrString)
{
    // Escape to url.
    CFStringRef cfsrPathEscaped =
    CFURLCreateStringByAddingPercentEscapes(NULL, cfsrString, NULL, NULL,    kCFStringEncodingUTF8);
    CFURLRef cfurUrl = CFURLCreateWithString(kCFAllocatorDefault, cfsrPathEscaped, NULL);
    CFRelease(cfsrPathEscaped);
    return cfurUrl;
}
