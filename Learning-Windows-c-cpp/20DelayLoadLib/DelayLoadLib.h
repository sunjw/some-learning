#ifndef _DELAY_LOAD_LIB_H_
#define _DELAY_LOAD_LIB_H_

#ifndef DELAYLOADLIBAPI
#define DELAYLOADLIBAPI extern "C" __declspec(dllimport)
#endif

DELAYLOADLIBAPI int fnLib();
DELAYLOADLIBAPI int fnLib2();

#endif
