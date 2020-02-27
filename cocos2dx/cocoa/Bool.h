#pragma once

#include "Object.h"


NS_CC_BEGIN

class CC_DLL Bool : public Object
{
	ACCEPT_VISITOR;

public:
	static Bool*	create(bool v);

public:
	bool			getValue() const;

private:
	Bool(bool v);


private:
	bool _bValue;
};



NS_CC_END