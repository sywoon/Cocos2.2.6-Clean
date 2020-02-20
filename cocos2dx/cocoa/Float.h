#pragma once

#include "Object.h"


NS_CC_BEGIN

class CC_DLL Float : public Object
{
public:
	static Float*	create(float v);

public:
	float			getValue() const;

private:
	Float(float v);


private:
	float _bValue;
};



NS_CC_END