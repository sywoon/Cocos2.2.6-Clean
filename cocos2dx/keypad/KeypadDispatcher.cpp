#include "KeypadDispatcher.h"
#include "KeypadDelegate.h"
#include "cocoa/Array.h"
#include "KeypadHandler.h"


NS_CC_BEGIN

KeypadDispatcher::KeypadDispatcher()
	: _bLocked(false)
	, _bToAdd(false)
	, _bToRemove(false)
{
	_pDelegates = Array::create();
	_pDelegates->retain();

	_pHandlersToAdd = ccCArrayNew(8);
	_pHandlersToRemove = ccCArrayNew(8);
}

KeypadDispatcher::~KeypadDispatcher()
{
	CC_SAFE_RELEASE(_pDelegates);
	if (_pHandlersToAdd)
	{
		ccCArrayFree(_pHandlersToAdd);
	}

	if (_pHandlersToRemove)
	{
		ccCArrayFree(_pHandlersToRemove);
	}
}



void KeypadDispatcher::addDelegate(KeypadDelegate* pDelegate)
{
	if (!pDelegate)
		return;

	if (!_bLocked)
	{
		forceAddDelegate(pDelegate);
	}
	else
	{
		ccCArrayAppendValue(_pHandlersToAdd, pDelegate);
		_bToAdd = true;
	}
}

void KeypadDispatcher::removeDelegate(KeypadDelegate* pDelegate)
{
	if (!pDelegate)
		return;
	
	if (!_bLocked)
	{
		forceRemoveDelegate(pDelegate);
	}
	else
	{
		ccCArrayAppendValue(_pHandlersToRemove, pDelegate);
		_bToRemove = true;
	}
}



void KeypadDispatcher::forceAddDelegate(KeypadDelegate* pDelegate)
{
	KeypadHandler* pHandler = KeypadHandler::handlerWithDelegate(pDelegate);

	if (pHandler)
	{
		_pDelegates->addObject(pHandler);
	}
}

void KeypadDispatcher::forceRemoveDelegate(KeypadDelegate* pDelegate)
{
	KeypadHandler* pHandler = NULL;
	Object* pObj = NULL;
	ARRAY_FOREACH(_pDelegates, pObj)
	{
		pHandler = (KeypadHandler*)pObj;
		if (pHandler && pHandler->getDelegate() == pDelegate)
		{
			_pDelegates->removeObject(pHandler);
			break;
		}
	}
}

bool KeypadDispatcher::dispatchKeypadMSG(ccKeypadMSGType nMsgType)
{
	KeypadHandler* pHandler = NULL;
	KeypadDelegate* pDelegate = NULL;

	_bLocked = true;

	if (_pDelegates->count() > 0)
	{
		Object* pObj = NULL;
		ARRAY_FOREACH(_pDelegates, pObj)
		{
			CC_BREAK_IF(!pObj);

			pHandler = (KeypadHandler*)pObj;
			pDelegate = pHandler->getDelegate();

			switch (nMsgType)
			{
			case kTypeBackClicked:
				pDelegate->keyBackClicked();
				break;
			case kTypeMenuClicked:
				pDelegate->keyMenuClicked();
				break;
			default:
				break;
			}
		}
	}

	_bLocked = false;
	if (_bToRemove)
	{
		_bToRemove = false;
		for (unsigned int i = 0; i < _pHandlersToRemove->num; ++i)
		{
			forceRemoveDelegate((KeypadDelegate*)_pHandlersToRemove->arr[i]);
		}
		ccCArrayRemoveAllValues(_pHandlersToRemove);
	}

	if (_bToAdd)
	{
		_bToAdd = false;
		for (unsigned int i = 0; i < _pHandlersToAdd->num; ++i)
		{
			forceAddDelegate((KeypadDelegate*)_pHandlersToAdd->arr[i]);
		}
		ccCArrayRemoveAllValues(_pHandlersToAdd);
	}

	return true;
}


NS_CC_END