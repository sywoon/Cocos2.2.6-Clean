#include "Bool.h"


NS_CC_BEGIN

Bool::Bool(bool v)
	: _bValue(v) {}


Bool* Bool::create(bool v)
{
	Bool* pRet = new Bool(v);
	if (pRet)
	{
		pRet->autorelease();
	}
	return pRet;
}

bool Bool::getValue() const 
{ 
	return _bValue; 
}




NS_CC_END