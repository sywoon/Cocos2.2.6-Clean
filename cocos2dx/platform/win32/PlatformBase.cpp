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


NS_CC_END


