#pragma once

#include "ccMacros.h"
#include "nodes/base_nodes/Node.h"
#include "touch/TouchDelegate.h"
#include "accelerometer/AccelerometerDelegate.h"
#include "keypad/KeypadDelegate.h"



NS_CC_BEGIN

typedef enum {
	kCCTouchesAllAtOnce,
	kCCTouchesOneByOne,
} ccTouchesMode;


class Touch;
class TouchEvent;
class Set;
class CC_DLL Layer : public Node, public TouchDelegate, public AccelerometerDelegate, public KeypadDelegate
{
public:
    Layer();
    virtual ~Layer();

    static Layer* create(void);


protected:
    virtual bool	init();


public:
	virtual void	onEnter();
	virtual void	onExit();


public:
	// touch
	virtual bool	isTouchEnabled();
	virtual void	setTouchEnabled(bool value);

	virtual void	setTouchMode(ccTouchesMode mode);
	virtual int		getTouchMode();

	virtual void	setTouchPriority(int priority);
	virtual int		getTouchPriority();

	virtual void	registerWithTouchDispatcher(void);

	// 都没实现  给继承类准备的
	virtual bool	touchBegan(Touch* pTouch, TouchEvent* pEvent);
	virtual void	touchMoved(Touch* pTouch, TouchEvent* pEvent);
	virtual void	touchEnded(Touch* pTouch, TouchEvent* pEvent);
	virtual void	touchCancelled(Touch* pTouch, TouchEvent* pEvent);

	virtual void	touchesBegan(Set* pTouches, TouchEvent* pEvent);
	virtual void	touchesMoved(Set* pTouches, TouchEvent* pEvent);
	virtual void	touchesEnded(Set* pTouches, TouchEvent* pEvent);
	virtual void	touchesCancelled(Set* pTouches, TouchEvent* pEvent);

	// accelerometer
	virtual bool	isAccelerometerEnabled();
	virtual void	setAccelerometerEnabled(bool value);

	virtual void	didAccelerate(Acceleration* pAccelerationValue);

	// keypad
	virtual bool	isKeypadEnabled();
	virtual void	setKeypadEnabled(bool value);

	virtual void	keyBackClicked(void);
	virtual void	keyMenuClicked(void);

private:
	bool			_bTouchEnabled;
	ccTouchesMode	_eTouchMode;
	int				_nTouchPriority;

	bool			_bAccelerometerEnabled;
	bool			_bKeypadEnabled;
};



NS_CC_END
