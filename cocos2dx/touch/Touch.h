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
	int		m_nId;
	bool	m_startPointCaptured;	
	Point	m_startPoint;			// ��һ��touch
	Point	m_point;				// ��ǰ
	Point	m_prevPoint;			// ��һ��
};



NS_CC_END
