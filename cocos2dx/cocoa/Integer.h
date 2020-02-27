#pragma once

#include "Object.h"


NS_CC_BEGIN

class CC_DLL Integer : public Object
{
	ACCEPT_VISITOR;

public:
	Integer(int v);
	static Integer* create(int v);

public:
	int				getValue() const;

private:
	int _bValue;
};



NS_CC_END