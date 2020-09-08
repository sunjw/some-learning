//
//  Util.h
//  NTFSTypeFixer
//
//  Created by Sun Junwen on 13-7-5.
//  Copyright (c) 2013 Sun Junwen. All rights reserved.
//

#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <CoreFoundation/CoreFoundation.h>

std::string CFStringRefToString(CFStringRef cfsrString);

CFStringRef StringToCFStringRef(const std::string& str);

CFURLRef CFStringRefToCFURLRef(CFStringRef cfsrString);

#endif /* defined(_UTIL_H_) */
