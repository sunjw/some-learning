//
//  TypeFixer.h
//  NTFSTypeFixer
//
//  Created by Sun Junwen on 13-7-5.
//  Copyright (c) 2013 Sun Junwen. All rights reserved.
//

#ifndef _TYPE_FIXER_
#define _TYPE_FIXER_

#include <string>

std::string GetFileType(const std::string& strPath);

void FixFileType(const std::string& strPath);

void StartFixThread();

#endif /* defined(_TYPE_FIXER_) */
