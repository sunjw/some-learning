//
//  FSWatcher.h
//  NTFSTypeFixer
//
//  Created by Sun Junwen on 13-7-5.
//  Copyright (c) 2013 Sun Junwen. All rights reserved.
//

#ifndef _FS_WATCHER_H_
#define _FS_WATCHER_H_

#include <string>
#include <CoreServices/CoreServices.h>

void StartFsEvent(const std::string& strPath);
void EndFsEvent();

#endif /* defined(_FS_WATCHER_H_) */
