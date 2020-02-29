#include "TouchHandler.h"
#include "TouchDelegate.h"
#include "platform/PlatformBase.h"
#include "cocoa/Set.h"


NS_CC_BEGIN


TouchHandler* TouchHandler::handlerWithDelegate(TouchDelegate* pDelegate, int nPriority)
{
	TouchHandler* pHandler = new TouchHandler();
	if (pHandler)
	{
		if (pHandler->initWithDelegate(pDelegate, nPriority))
		{
			pHandler->autorelease();
		}
		else
		{
			CC_SAFE_RELEASE_NULL(pHandler);
		}
	}

	return pHandler;
}

TouchHandler::TouchHandler(void)
	: _pDelegate(NULL)
	, _nPriority(0)
{
}

TouchHandler::~TouchHandler(void)
{
	releaseDelegate();
}

TouchDelegate* TouchHandler::getDelegate(void)
{
	return _pDelegate;
}

void TouchHandler::setDelegate(TouchDelegate* pDelegate)
{
	releaseDelegate();
	initWithDelegate(pDelegate, 0);
}


int TouchHandler::getPriority(void)
{
	return _nPriority;
}

void TouchHandler::setPriority(int nPriority)
{
	_nPriority = nPriority;
}

void TouchHandler::releaseDelegate()
{
	if (_pDelegate)
	{
		dynamic_cast<Object*>(_pDelegate)->release();
		_pDelegate = NULL;
	}
}

bool TouchHandler::initWithDelegate(TouchDelegate* pDelegate, int nPriority)
{
	CCAssert(pDelegate != NULL, "touch delegate should not be null");

	_pDelegate = pDelegate;
	dynamic_cast<Object*>(pDelegate)->retain();

	_nPriority = nPriority;

	return true;
}



StandardTouchHandler* StandardTouchHandler::handlerWithDelegate(TouchDelegate* pDelegate, int nPriority)
{
	StandardTouchHandler* pHandler = new StandardTouchHandler();

	if (pHandler)
	{
		if (pHandler->initWithDelegate(pDelegate, nPriority))
		{
			pHandler->autorelease();
		}
		else
		{
			CC_SAFE_RELEASE_NULL(pHandler);
		}
	}

	return pHandler;
}




TargetedTouchHandler* TargetedTouchHandler::handlerWithDelegate(TouchDelegate* pDelegate, int nPriority, bool bSwallow)
{
	TargetedTouchHandler* pHandler = new TargetedTouchHandler();
	if (pHandler)
	{
		if (pHandler->initWithDelegate(pDelegate, nPriority, bSwallow))
		{
			pHandler->autorelease();
		}
		else
		{
			CC_SAFE_RELEASE_NULL(pHandler);
		}
	}

	return pHandler;
}


TargetedTouchHandler::TargetedTouchHandler(void)
	: _bSwallowsTouches(false)
	, _pClaimedTouches(NULL)
{
}

TargetedTouchHandler::~TargetedTouchHandler(void)
{
	CC_SAFE_RELEASE(_pClaimedTouches);
}

bool TargetedTouchHandler::isSwallowsTouches(void)
{
	return _bSwallowsTouches;
}

void TargetedTouchHandler::setSwallowsTouches(bool bSwallowsTouches)
{
	_bSwallowsTouches = bSwallowsTouches;
}

Set* TargetedTouchHandler::getClaimedTouches(void)
{
	return _pClaimedTouches;
}

bool TargetedTouchHandler::initWithDelegate(TouchDelegate* pDelegate, int nPriority, bool bSwallow)
{
	if (TouchHandler::initWithDelegate(pDelegate, nPriority))
	{
		_pClaimedTouches = new Set();
		_bSwallowsTouches = bSwallow;

		return true;
	}

	return false;
}



NS_CC_END