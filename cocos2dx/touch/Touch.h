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

	// ���ڴ������Ļ����
	void	setTouchInfo(int id, float x, float y);
	int		getID() const;

	Point	getLocation() const;
	Point	getPreviousLocation() const;
	Point	getStartLocation() const;

	Point	getDelta() const;   //�õ��������ϵ�µľ���

	// ��Ļ����ϵ�µ�ԭ������
	Point	getLocationInView() const;
	Point	getPreviousLocationInView() const;
	Point	getStartLocationInView() const;


private:
	int		_nId;
	bool	_startPointCaptured;	
	Point	_startPoint;			// ��һ��touch
	Point	_point;				// ��ǰ
	Point	_prevPoint;			// ��һ��
};



NS_CC_END
