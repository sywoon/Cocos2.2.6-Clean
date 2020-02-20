#include "Integer.h"


NS_CC_BEGIN

Integer::Integer(int v)
	: _bValue(v) {}


Integer* Integer::create(int v)
{
	Integer* pRet = new Integer(v);
	if (pRet)
	{
		pRet->autorelease();
	}
	return pRet;
}

int Integer::getValue() const
{
	return _bValue;
}




NS_CC_END