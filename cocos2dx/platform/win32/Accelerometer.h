#pragma once
#include "ccMacros.h"
#include "accelerometer/AccelerometerDelegate.h"

NS_CC_BEGIN


class CC_DLL Accelerometer
{
public:
	Accelerometer();
	~Accelerometer();

	void	setDelegate(AccelerometerDelegate* pDelegate);

	void	update(double x, double y, double z, double timestamp);


private:
	Acceleration			_obAccelerationValue;
	AccelerometerDelegate*  _pAccelDelegate;
};



NS_CC_END