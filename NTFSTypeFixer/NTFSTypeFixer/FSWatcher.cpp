//
//  FSWatcher.cpp
//  NTFSTypeFixer
//
//  Created by Sun Junwen on 13-7-5.
//  Copyright (c) 2013年 Sun Junwen. All rights reserved.
//

#include "FSWatcher.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <CoreServices/CoreServices.h>

#include "Util.h"
#include "FileQueue.h"

using namespace std;

static FSEventStreamRef s_streamRef;

static void FsEventCallback(ConstFSEventStreamRef streamRef,
                     void *clientCallBackInfo,
                     size_t numEvents,
                     void *eventPaths,
                     const FSEventStreamEventFlags eventFlags[],
                     const FSEventStreamEventId eventIds[])
{
    char **szPaths = (char **)eventPaths;
    
    struct stat fileStat;
    for(int i = 0; i < numEvents; i++)
    {
        const char *szPath = szPaths[i];
        //printf("Path %s\n", szPath);
        
        LockFileQueue();
        
        if(stat(szPath, &fileStat) == 0)
        {
            if(fileStat.st_mode & S_IFREG)
                InsertFile(szPath); // a file
            else if(fileStat.st_mode & S_IFDIR)
                InsertDir(szPath, true, true); // a directory
        }
        
        UnlockFileQueue();
    }
}

void StartFsEvent(const string& strPath)
{
    /*
     Define variables and create a CFArray object containing
     CFString objects containing paths to watch.
     */
    CFStringRef cfsrPath = StringToCFStringRef(strPath);
    CFArrayRef pathArray = CFArrayCreate(NULL, (const void **)&cfsrPath, 1, NULL);
    
    CFAbsoluteTime latency = 1.0; /* Latency in seconds */
    
    /* Create the stream, passing in a callback */
    s_streamRef = FSEventStreamCreate(NULL,
                                      &FsEventCallback,
                                      NULL,
                                      pathArray,
                                      kFSEventStreamEventIdSinceNow, /* Or a previous event ID */
                                      latency,
                                      kFSEventStreamCreateFlagNone /* Flags explained in reference */
                                      );
    
    /* Create the stream before calling this. */
    FSEventStreamScheduleWithRunLoop(s_streamRef,
                                     CFRunLoopGetCurrent(),
                                     kCFRunLoopDefaultMode);
    
    FSEventStreamStart(s_streamRef);
    
    CFRunLoopRun();
}

void EndFsEvent()
{
    FSEventStreamStop(s_streamRef);
    FSEventStreamInvalidate(s_streamRef);
    FSEventStreamRelease(s_streamRef);
}
