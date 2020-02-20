#pragma once

#include <string>
#include "ccMacros.h"
#include "Object.h"



NS_CC_BEGIN


class CC_DLL String : public Object
{
public:
	String();
	String(const char* str);
	String(const std::string& str);
	String(const String& str);

	virtual ~String();

	static String* create(const std::string& str);

	// ass sprintf the default buffer size is (1024*100) bytes kMaxStringLen
	static String* createWithFormat(const char* format, ...) CC_FORMAT_PRINTF(1, 2);

	// create a string with binary data
	static String* createWithData(const unsigned char* pData, unsigned long nLen);


public:
	String&			operator= (const String& other);

	bool			initWithFormat(const char* format, ...) CC_FORMAT_PRINTF(2, 3);

	int				intValue() const;
	unsigned int	uintValue() const;
	float			floatValue() const;
	double			doubleValue() const;
	bool			boolValue() const;
	const char*		getCString() const;

	unsigned int	length() const;
	int				compare(const char*) const;

public:
	virtual bool	isEqual(const Object* pObject);


private:
	bool initWithFormatAndValist(const char* format, va_list ap);


public:
	std::string _sString;
};




NS_CC_END
