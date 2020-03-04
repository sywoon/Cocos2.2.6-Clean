#include "CommonWin32.h"


NS_CC_BEGIN


std::string utf8Togbk(const char* src)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
	unsigned short* wszGBK = new unsigned short[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)src, -1, (LPWSTR)wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, szGBK, len, NULL, NULL);

	std::string strTemp(szGBK);
	if (strTemp.find('?') != std::string::npos)
	{
		strTemp.assign(src);
	}
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;
}


wchar_t* utf8ToWideChar(std::string nString)
{
	wchar_t* pwszBuffer = NULL;
	do
	{
		if (nString.size() < 0)
			break;

		// utf-8 to utf-16
		int nLen = nString.size();
		int nBufLen = nLen + 1;
		pwszBuffer = new wchar_t[nBufLen];
		CC_BREAK_IF(!pwszBuffer);

		memset(pwszBuffer, 0, nBufLen);
		nLen = MultiByteToWideChar(CP_UTF8, 0, nString.c_str(), nLen, pwszBuffer, nBufLen);
		pwszBuffer[nLen] = '\0';
	} while (0);

	return pwszBuffer;
}



NS_CC_END
