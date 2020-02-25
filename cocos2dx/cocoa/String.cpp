#include "String.h"


NS_CC_BEGIN

#define kMaxStringLen (1024*100)


String::String()
	:_sString("")
{}

String::String(const char* str)
	: _sString(str)
{}

String::String(const std::string& str)
	: _sString(str)
{}

String::String(const String& str)
	: _sString(str.getCString())
{}

String::~String()
{
	_sString.clear();
}



String* String::create(const std::string& str)
{
	String* pRet = new String(str);
	pRet->autorelease();
	return pRet;
}

String* String::createWithData(const unsigned char* pData, unsigned long nLen)
{
	String* pRet = NULL;
	if (pData != NULL)
	{
		char* pStr = (char*)malloc(nLen + 1);
		if (pStr != NULL)
		{
			pStr[nLen] = '\0';
			if (nLen > 0)
			{
				memcpy(pStr, pData, nLen);
			}

			pRet = String::create(pStr);
			free(pStr);
		}
	}
	return pRet;
}

String* String::createWithFormat(const char* format, ...)
{
	String* pRet = String::create("");
	va_list ap;
	va_start(ap, format);
	pRet->initWithFormatAndValist(format, ap);
	va_end(ap);

	return pRet;
}


String& String::operator= (const String& other)
{
	_sString = other._sString;
	return *this;
}


bool String::initWithFormat(const char* format, ...)
{
	bool bRet = false;
	_sString.clear();

	va_list ap;
	va_start(ap, format);

	bRet = initWithFormatAndValist(format, ap);

	va_end(ap);

	return bRet;
}

int String::intValue() const
{
	if (length() == 0)
	{
		return 0;
	}
	return atoi(_sString.c_str());
}

unsigned int String::uintValue() const
{
	if (length() == 0)
	{
		return 0;
	}
	return (unsigned int)atoi(_sString.c_str());
}

float String::floatValue() const
{
	if (length() == 0)
	{
		return 0.0f;
	}
	return (float)atof(_sString.c_str());
}

double String::doubleValue() const
{
	if (length() == 0)
	{
		return 0.0;
	}
	return atof(_sString.c_str());
}

bool String::boolValue() const
{
	if (length() == 0)
	{
		return false;
	}

	if (0 == strcmp(_sString.c_str(), "0") || 0 == strcmp(_sString.c_str(), "false"))
	{
		return false;
	}
	return true;
}

const char* String::getCString() const
{
	return _sString.c_str();
}


unsigned int String::length() const
{
	return _sString.length();
}

int String::compare(const char* pStr) const
{
	return strcmp(getCString(), pStr);
}


bool String::isEqual(const Object* pObject)
{
	bool bRet = false;
	const String* pStr = dynamic_cast<const String*>(pObject);
	if (pStr != NULL)
	{
		if (0 == _sString.compare(pStr->_sString))
		{
			bRet = true;
		}
	}
	return bRet;
}

Object* String::cloneSelf(void)
{
	String* p = new String(*this);

	return p;
}


bool String::initWithFormatAndValist(const char* format, va_list ap)
{
	int len = vsnprintf(0, 0, format, ap) + 1; //add \0
	len = MIN(len, kMaxStringLen);

	bool bRet = false;
	char* pBuf = (char*)malloc(len);
	if (pBuf != NULL)
	{
		vsnprintf(pBuf, len, format, ap);
		_sString = pBuf;
		free(pBuf);
		bRet = true;
	}
	return bRet;
}





NS_CC_END
