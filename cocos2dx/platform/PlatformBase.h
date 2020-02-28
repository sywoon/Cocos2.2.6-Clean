#pragma once

#include "ccMacros.h"


NS_CC_BEGIN

static const int kMaxLogLen = 16 * 1024;


void CC_DLL CCLog(const char* pszFormat, ...) CC_FORMAT_PRINTF(1, 2);

void CC_DLL CCMessageBox(const char* pszMsg, const char* pszTitle);



struct CC_DLL cc_timeval
{
#ifdef __native_client__
	time_t    tv_sec;        // seconds
#else
	long    tv_sec;        // seconds
#endif
	int		tv_usec;    // microSeconds
};

class CC_DLL CCTime
{
public:
	static int		gettimeofdayCocos2d(struct cc_timeval* tp, void* tzp);
	static double	timersubCocos2d(struct cc_timeval* start, struct cc_timeval* end);
};



NS_CC_END