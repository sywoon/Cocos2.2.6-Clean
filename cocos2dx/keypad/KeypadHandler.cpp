#include "KeypadHandler.h"
#include "KeypadDelegate.h"
#include "platform/PlatformBase.h"


NS_CC_BEGIN

KeypadHandler* KeypadHandler::handlerWithDelegate(KeypadDelegate* pDelegate)
{
	KeypadHandler* pHandler = new KeypadHandler();

	if (pHandler)
	{
		if (pHandler->initWithDelegate(pDelegate))
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

KeypadHandler::KeypadHandler()
	: _pDelegate(NULL)
{
}

KeypadHandler::~KeypadHandler()
{
	releaseDelegate();
}


KeypadDelegate* KeypadHandler::getDelegate()
{
	return _pDelegate;
}

void KeypadHandler::setDelegate(KeypadDelegate* pDelegate)
{
	releaseDelegate();

	initWithDelegate(pDelegate);
}


void KeypadHandler::releaseDelegate()
{
	if (_pDelegate)
	{
		// KeypadDelegate 实际是作为Interface使用的 实际子类从Object中继承
		dynamic_cast<Object*>(_pDelegate)->release();
	}
}

bool KeypadHandler::initWithDelegate(KeypadDelegate* pDelegate)
{
	CCAssert(pDelegate != NULL, "It's a wrong delegate!");

	_pDelegate = pDelegate;
	dynamic_cast<Object*>(pDelegate)->retain();

	return true;
}



NS_CC_END


