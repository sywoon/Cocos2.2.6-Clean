#pragma once
#include "ccMacros.h"
#include "cocoa/Object.h"
#include "cocoa/Geometry.h"



NS_CC_BEGIN

#define CC_MAX_TOUCHES  5


class CC_DLL TouchEvent : public Object
{
};


class CC_DLL Touch : public Object
{
public:
	Touch();

	// 窗口传入的屏幕坐标
	void	setTouchInfo(int id, float x, float y);
	int		getID() const;

	Point	getLocation() const;
	Point	getPreviousLocation() const;
	Point	getStartLocation() const;

	Point	getDelta() const;   //得到设计坐标系下的距离

	// 屏幕坐标系下的原生坐标
	Point	getLocationInView() const;
	Point	getPreviousLocationInView() const;
	Point	getStartLocationInView() const;


private:
	int		_nId;
	bool	_startPointCaptured;	
	Point	_startPoint;			// 第一次touch
	Point	_point;				// 当前
	Point	_prevPoint;			// 上一个
};



NS_CC_END
