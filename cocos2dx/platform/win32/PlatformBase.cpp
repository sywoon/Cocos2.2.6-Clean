#include "PlatformBase.h"
#include <stdio.h>


NS_CC_BEGIN

#define MAX_LEN         (cocos2d::kMaxLogLen + 1)

void CCLog(const char* pszFormat, ...)
{
	char szBuf[MAX_LEN];

	va_list ap;
	va_start(ap, pszFormat);
	vsnprintf_s(szBuf, MAX_LEN, MAX_LEN, pszFormat, ap);
	va_end(ap);

	OutputDebugString(szBuf);
	OutputDebugStringA("\n");

	printf("%s\n", szBuf);
}

void CCMessageBox(const char* pszMsg, const char* pszTitle)
{
	MessageBoxA(NULL, pszMsg, pszTitle, MB_OK);
}


static int gettimeofday(struct timeval* val, struct timezone*)
{
	if (val)
	{
		LARGE_INTEGER liTime, liFreq;
		QueryPerformanceFrequency(&liFreq);
		QueryPerformanceCounter(&liTime);
		val->tv_sec = (long)(liTime.QuadPart / liFreq.QuadPart);
		val->tv_usec = (long)(liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val->tv_sec * 1000000.0);
	}
	return 0;
}


int CCTime::gettimeofdayCocos2d(struct cc_timeval* tp, void* tzp)
{
	CC_UNUSED_PARAM(tzp);
	if (tp)
	{
		gettimeofday((struct timeval*)tp, 0);
	}
	return 0;
}

double CCTime::timersubCocos2d(struct cc_timeval* start, struct cc_timeval* end)
{
	if (!start || !end)
	{
		return 0;
	}

	return ((end->tv_sec * 1000.0 + end->tv_usec / 1000.0) - (start->tv_sec * 1000.0 + start->tv_usec / 1000.0));
}



NS_CC_END


