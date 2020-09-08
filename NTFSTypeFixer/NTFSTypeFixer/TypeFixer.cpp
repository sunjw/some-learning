//
//  TypeFixer.cpp
//  NTFSTypeFixer
//
//  Created by Sun Junwen on 13-7-5.
//  Copyright (c) 2013 Sun Junwen. All rights reserved.
//

#include "TypeFixer.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <set>
#include <CoreServices/CoreServices.h>

#include "FileQueue.h"
#include "Util.h"

#define BROK_TYPE "brok"

using namespace std;

typedef set<string> StrSet;

string GetFileType(const string& strPath)
{
    string ret;
    
    FSCatalogInfo fsCatInfo;
    FSRef fsRef;
    
    if(!FSPathMakeRef((const UInt8 *)strPath.c_str(), &fsRef, false))
    {
        FSGetCatalogInfo(&fsRef, kFSCatInfoFinderInfo, &fsCatInfo, NULL, NULL, NULL);
        FileInfo *info = (FileInfo*)fsCatInfo.finderInfo;
        //info->fileType = 'EFGh';
        //FSSetCatalogInfo(&fsRef, kFSCatInfoFinderInfo, &fsCatInfo);
        
        CFStringRef cfsRef = UTCreateStringForOSType(info->fileType);
        ret = CFStringRefToString(cfsRef);
    }
    
    return ret;
}

void FixFileType(const string& strPath)
{
    CFStringRef cfsrEmpty = StringToCFStringRef("");
    OSType ostEmpty = UTGetOSTypeFromString(cfsrEmpty);
    
    FSCatalogInfo fsCatInfo;
    FSRef fsRef;
    
    if(!FSPathMakeRef((const UInt8 *)strPath.c_str(), &fsRef, false))
    {
        FSGetCatalogInfo(&fsRef, kFSCatInfoFinderInfo, &fsCatInfo, NULL, NULL, NULL);
        FileInfo *info = (FileInfo*)fsCatInfo.finderInfo;
        info->fileCreator = ostEmpty;
        info->fileType = ostEmpty;
        FSSetCatalogInfo(&fsRef, kFSCatInfoFinderInfo, &fsCatInfo);
    }
}

static void *FixWorker(void *arg)
{
    string strPath, strType;
    struct stat fileStat;
    
    StrSet fileset;
    struct timeval tvLastSet = {0}, tvCurrent;
    
    while(1)
    {
        LockFileQueue();
        
        gettimeofday(&tvCurrent, NULL);
        if(tvLastSet.tv_sec == 0 ||
           tvCurrent.tv_sec - tvLastSet.tv_sec > 7200)
        {
            fileset.clear();
            tvLastSet = tvCurrent;
        }
        
        // Pop a file
        strPath = PopFile();
        if(strPath != "" && fileset.find(strPath) == fileset.end())
        {
            //printf("PopFile:\n%s\n", strPath.c_str());
            stat(strPath.c_str(), &fileStat);
            if(fileStat.st_mode & S_IFREG)
            {
                // file
                strType = GetFileType(strPath);
                if(strType == BROK_TYPE)
                {
                    printf("FixFileType:\n%s\n", strPath.c_str());
                    FixFileType(strPath);
                }
            }
            else if(fileStat.st_mode & S_IFDIR)
            {
                // dir
                InsertDir(strPath, false, false);
            }
            fileset.insert(strPath);
        }
        
        UnlockFileQueue();
        
        usleep(50000); // 50ms
    }
    
    return 0;
}

void StartFixThread()
{
    pthread_t pthId;
    pthread_create(&pthId, NULL, FixWorker, NULL);
}
