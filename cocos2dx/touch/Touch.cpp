#include "Touch.h"
#include "Director.h"
#include "cocoa/extend/PointExtend.h"

NS_CC_BEGIN


Touch::Touch()
	: m_nId(0)
	, m_startPointCaptured(false)
{}

void Touch::setTouchInfo(int id, float x, float y)
{
	m_nId = id;
	m_prevPoint = m_point;
	m_point.x = x;
	m_point.y = y;

	if (!m_startPointCaptured)
	{
		m_startPoint = m_point;
		m_startPointCaptured = true;
	}
}

int Touch::getID() const
{
	return m_nId;
}


Point Touch::getLocation() const
{
	return Director::sharedDirector()->convertToGL(m_point);
}

Point Touch::getPreviousLocation() const
{
	return Director::sharedDirector()->convertToGL(m_prevPoint);
}

// returns the start touch location in OpenGL coordinates
Point Touch::getStartLocation() const
{
	return Director::sharedDirector()->convertToGL(m_startPoint);
}

Point Touch::getDelta() const
{
	return ccpSub(getLocation(), getPreviousLocation());
}

Point Touch::getLocationInView() const
{
	return m_point;
}

Point Touch::getPreviousLocationInView() const
{
	return m_prevPoint;
}

Point Touch::getStartLocationInView() const
{
	return m_startPoint;
}



NS_CC_END
