#pragma once

#include "Object.h"


NS_CC_BEGIN

class CC_DLL Double : public Object
{
	ACCEPT_VISITOR;

public:
	static Double*  create(double v);

public:
	double			getValue() const;

private:
	Double(double v);


private:
	double _bValue;
};



NS_CC_END