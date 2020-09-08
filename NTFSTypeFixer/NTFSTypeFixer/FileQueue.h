//
//  FileQueue.h
//  NTFSTypeFixer
//
//  Created by Sun Junwen on 13-7-5.
//  Copyright (c) 2013 Sun Junwen. All rights reserved.
//

#ifndef _FILE_QUEUE_H_
#define _FILE_QUEUE_H_

#include <string>
#include <list>

typedef std::list<std::string> StrList;

void LockFileQueue();

void UnlockFileQueue();

std::string PopFile();

/*
 Must LockFileQueue() first,
 and UnlockFileQueue() after finished opertion.
 */
StrList::iterator InsertFile(const std::string& strFilePath);
StrList::iterator InsertFile(const std::string& strFilePath,
                             StrList::const_iterator itrWhere);

StrList::iterator InsertDir(const std::string& strDirPath,
                            bool fInsertFront, bool fAddSubDir);
/*
 Must LockFileQueue() first,
 and UnlockFileQueue() after finished opertion.
 */

#endif /* defined(_FILE_QUEUE_H_) */
