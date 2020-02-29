#pragma once

#include "ccMacros.h"


NS_CC_BEGIN

class Touch;
class TouchEvent;
class Set;
class CC_DLL TouchDelegate
{
public:
	TouchDelegate() {}
	virtual ~TouchDelegate() {}

	virtual bool touchBegan(Touch* pTouch, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent); return false; };
	virtual void touchMoved(Touch* pTouch, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent); }
	virtual void touchEnded(Touch* pTouch, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent); }
	virtual void touchCancelled(Touch* pTouch, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent); }

	virtual void touchesBegan(Set* pTouches, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouches); CC_UNUSED_PARAM(pEvent); }
	virtual void touchesMoved(Set* pTouches, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouches); CC_UNUSED_PARAM(pEvent); }
	virtual void touchesEnded(Set* pTouches, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouches); CC_UNUSED_PARAM(pEvent); }
	virtual void touchesCancelled(Set* pTouches, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouches); CC_UNUSED_PARAM(pEvent); }
};

// ���㴥��
class CC_DLL OneTouchDelegate : public TouchDelegate
{
public:
    // Return YES to claim the touch.   ����true �������touch �����move����Ϣ���ᴫ���������
    virtual bool touchBegan(Touch* pTouch, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent); return false; };
    virtual void touchMoved(Touch* pTouch, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent); }
    virtual void touchEnded(Touch* pTouch, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent); }
    virtual void touchCancelled(Touch* pTouch, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouch); CC_UNUSED_PARAM(pEvent); }
};


// ��㴥��
class CC_DLL MultiTouchDelegate : public TouchDelegate
{
public:
	virtual void touchesBegan(Set* pTouches, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouches); CC_UNUSED_PARAM(pEvent); }
	virtual void touchesMoved(Set* pTouches, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouches); CC_UNUSED_PARAM(pEvent); }
	virtual void touchesEnded(Set* pTouches, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouches); CC_UNUSED_PARAM(pEvent); }
	virtual void touchesCancelled(Set* pTouches, TouchEvent* pEvent) { CC_UNUSED_PARAM(pTouches); CC_UNUSED_PARAM(pEvent); }
};



NS_CC_END