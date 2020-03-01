#pragma once

#include "ccMacros.h"

NS_CC_BEGIN

class Acceleration
{
public:
	double x;
	double y;
	double z;

	double timestamp;
};


class CC_DLL AccelerometerDelegate
{
public:
	virtual void didAccelerate(Acceleration* pAccelerationValue) { CC_UNUSED_PARAM(pAccelerationValue); }
};






NS_CC_END