#include "Touch.h"
#include "Director.h"
#include "cocoa/extend/PointExtend.h"

NS_CC_BEGIN


Touch::Touch()
	: _nId(0)
	, _startPointCaptured(false)
{}

void Touch::setTouchInfo(int id, float x, float y)
{
	_nId = id;
	_prevPoint = _point;
	_point.x = x;
	_point.y = y;

	if (!_startPointCaptured)
	{
		_startPoint = _point;
		_startPointCaptured = true;
	}
}

int Touch::getID() const
{
	return _nId;
}


Point Touch::getLocation() const
{
	return Director::sharedDirector()->convertToGL(_point);
}

Point Touch::getPreviousLocation() const
{
	return Director::sharedDirector()->convertToGL(_prevPoint);
}

// returns the start touch location in OpenGL coordinates
Point Touch::getStartLocation() const
{
	return Director::sharedDirector()->convertToGL(_startPoint);
}

Point Touch::getDelta() const
{
	return ccpSub(getLocation(), getPreviousLocation());
}

Point Touch::getLocationInView() const
{
	return _point;
}

Point Touch::getPreviousLocationInView() const
{
	return _prevPoint;
}

Point Touch::getStartLocationInView() const
{
	return _startPoint;
}



NS_CC_END
