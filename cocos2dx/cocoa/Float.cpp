#include "Float.h"


NS_CC_BEGIN

Float::Float(float v)
	: _bValue(v) {}


Float* Float::create(float v)
{
	Float* pRet = new Float(v);
	if (pRet)
	{
		pRet->autorelease();
	}
	return pRet;
}

float Float::getValue() const
{
	return _bValue;
}




NS_CC_END