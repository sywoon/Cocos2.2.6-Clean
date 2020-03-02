#pragma once

#include "ccMacros.h"

#include <typeinfo>
#include <ctype.h>
#include <string.h>



NS_CC_BEGIN


class ClassInfo
{
public:
	virtual long getClassTypeId() = 0;
};

static inline unsigned int getHashCodeByString(const char *key)
{
	unsigned int len = strlen(key);
	const char *end=key+len;
	unsigned int hash;

	for (hash = 0; key < end; key++)
	{
		hash *= 16777619;
		hash ^= (unsigned int) (unsigned char) toupper(*key);
	}
	return (hash);
}




NS_CC_END
