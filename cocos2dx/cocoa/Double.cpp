#include "Double.h"


NS_CC_BEGIN

Double::Double(double v)
	: _bValue(v) {}


Double* Double::create(double v)
{
	Double* pRet = new Double(v);
	if (pRet)
	{
		pRet->autorelease();
	}
	return pRet;
}

double Double::getValue() const
{
	return _bValue;
}




NS_CC_END