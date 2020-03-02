#include "Layer.h"

#include "cocoa/Set.h"
#include "touch/Touch.h"
#include "touch/TouchDispatcher.h"
#include "keypad/KeypadDispatcher.h"

#include "Director.h"


NS_CC_BEGIN


Layer* Layer::create()
{
	Layer* pRet = new Layer();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}

Layer::Layer()
	: _bTouchEnabled(false)
	, _eTouchMode(kCCTouchesAllAtOnce)
	, _nTouchPriority(0)
	, _bAccelerometerEnabled(false)
	, _bKeypadEnabled(false)
{
	_bIgnoreAnchorPointForPosition = true;
	setAnchorPoint(ccp(0.5f, 0.5f));
}

Layer::~Layer()
{
}

bool Layer::init()
{
	bool bRet = false;
	do
	{
		Director* pDirector;
		CC_BREAK_IF(!(pDirector = Director::sharedDirector()));
		this->setContentSize(pDirector->getWinSize());
		bRet = true;
	} while (0);
	return bRet;
}

void Layer::onEnter()
{
	Director* pDirector = Director::sharedDirector();

	// register 'parent' nodes first
	// since events are propagated in reverse order
	if (_bTouchEnabled)
	{
		this->registerWithTouchDispatcher();
	}

	// then iterate over all the children
	Node::onEnter();

	// add this layer to concern the Accelerometer Sensor
	if (_bAccelerometerEnabled)
	{
		pDirector->getAccelerometer()->setDelegate(this);
	}

	// add this layer to concern the keypad msg
	if (_bKeypadEnabled)
	{
		pDirector->getKeypadDispatcher()->addDelegate(this);
	}
}

void Layer::onExit()
{
	Director* pDirector = Director::sharedDirector();
	if (_bTouchEnabled)
	{
		pDirector->getTouchDispatcher()->removeDelegate(this);
		// [lua]:don't unregister script touch handler, or the handler will be destroyed
		// unregisterScriptTouchHandler();
	}

	if (_bAccelerometerEnabled)
	{
		pDirector->getAccelerometer()->setDelegate(NULL);
	}

	if (_bKeypadEnabled)
	{
		pDirector->getKeypadDispatcher()->removeDelegate(this);
	}

	Node::onExit();
}



/////////////////////////////////////
// touch
//
bool Layer::isTouchEnabled()
{
	return _bTouchEnabled;
}

void Layer::setTouchEnabled(bool enabled)
{
	if (_bTouchEnabled != enabled)
	{
		_bTouchEnabled = enabled;
		if (_bRunning)
		{
			if (enabled)
			{
				this->registerWithTouchDispatcher();
			}
			else
			{
				Director::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
			}
		}
	}
}

void Layer::setTouchMode(ccTouchesMode mode)
{
	if (_eTouchMode != mode)
	{
		_eTouchMode = mode;

		if (_bTouchEnabled)
		{
			setTouchEnabled(false);
			setTouchEnabled(true);
		}
	}
}

int Layer::getTouchMode()
{
	return _eTouchMode;
}


void Layer::setTouchPriority(int priority)
{
	if (_nTouchPriority != priority)
	{
		_nTouchPriority = priority;

		if (_bTouchEnabled)
		{
			setTouchEnabled(false);
			setTouchEnabled(true);
		}
	}
}

int Layer::getTouchPriority()
{
	return _nTouchPriority;
}


void Layer::registerWithTouchDispatcher()
{
	TouchDispatcher* pDispatcher = Director::sharedDirector()->getTouchDispatcher();

	if (_eTouchMode == kCCTouchesAllAtOnce) {
		pDispatcher->addStandardDelegate(this, 0);
	}
	else {
		pDispatcher->addTargetedDelegate(this, _nTouchPriority, true);
	}
}


bool Layer::touchBegan(Touch* pTouch, TouchEvent* pEvent)
{
	CC_UNUSED_PARAM(pTouch);
	CC_UNUSED_PARAM(pEvent);
	CCAssert(false, "Layer#touchBegan override me");
	return true;
}

void Layer::touchMoved(Touch* pTouch, TouchEvent* pEvent)
{
	CC_UNUSED_PARAM(pTouch);
	CC_UNUSED_PARAM(pEvent);
}

void Layer::touchEnded(Touch* pTouch, TouchEvent* pEvent)
{
	CC_UNUSED_PARAM(pTouch);
	CC_UNUSED_PARAM(pEvent);
}

void Layer::touchCancelled(Touch* pTouch, TouchEvent* pEvent)
{
	CC_UNUSED_PARAM(pTouch);
	CC_UNUSED_PARAM(pEvent);
}



void Layer::touchesBegan(Set* pTouches, TouchEvent* pEvent)
{
	CC_UNUSED_PARAM(pTouches);
	CC_UNUSED_PARAM(pEvent);
}

void Layer::touchesMoved(Set* pTouches, TouchEvent* pEvent)
{
	CC_UNUSED_PARAM(pTouches);
	CC_UNUSED_PARAM(pEvent);
}

void Layer::touchesEnded(Set* pTouches, TouchEvent* pEvent)
{
	CC_UNUSED_PARAM(pTouches);
	CC_UNUSED_PARAM(pEvent);
}

void Layer::touchesCancelled(Set* pTouches, TouchEvent* pEvent)
{
	CC_UNUSED_PARAM(pTouches);
	CC_UNUSED_PARAM(pEvent);
}

/////////////////////////////////////
// accelerometer
//
bool Layer::isAccelerometerEnabled()
{
	return _bAccelerometerEnabled;
}

void Layer::setAccelerometerEnabled(bool enabled)
{
	if (enabled != _bAccelerometerEnabled)
	{
		_bAccelerometerEnabled = enabled;

		if (_bRunning)
		{
			Director* pDirector = Director::sharedDirector();
			pDirector->getAccelerometer()->setDelegate(enabled ? this : NULL);
		}
	}
}

void Layer::didAccelerate(Acceleration* pAccelerationValue)
{
	CC_UNUSED_PARAM(pAccelerationValue);
}


/////////////////////////////////////
// keypad
//
bool Layer::isKeypadEnabled()
{
	return _bKeypadEnabled;
}

void Layer::setKeypadEnabled(bool enabled)
{
	if (enabled != _bKeypadEnabled)
	{
		_bKeypadEnabled = enabled;

		if (_bRunning)
		{
			Director* pDirector = Director::sharedDirector();
			if (enabled)
			{
				pDirector->getKeypadDispatcher()->addDelegate(this);
			}
			else
			{
				pDirector->getKeypadDispatcher()->removeDelegate(this);
			}
		}
	}
}

void Layer::keyBackClicked(void)
{
}

void Layer::keyMenuClicked(void)
{
}




NS_CC_END