#include "TouchDispatcher.h"
#include "Touch.h"
#include "TouchHandler.h"
#include "TouchDelegate.h"
#include "cocoa/Array.h"
#include "cocoa/data_support/ccCArray.h"
#include "platform/PlatformBase.h"
#include <algorithm>



NS_CC_BEGIN

static int less(const Object* p1, const Object* p2)
{
	return ((TouchHandler*)p1)->getPriority() < ((TouchHandler*)p2)->getPriority();
}


TouchDispatcher::TouchDispatcher()
	: _bDispatchEvents(true)
	, _bLocked(false)
	, _bToQuit(false)
	, _bToAdd(false)
	, _bToRemove(false)
{
	_pOneHandlers = Array::createWithCapacity(8);
	_pOneHandlers->retain();
	_pMultiHandlers = Array::createWithCapacity(4);
	_pMultiHandlers->retain();

	_pHandlersToAdd = Array::createWithCapacity(8);
	_pHandlersToAdd->retain();
	_pDelegateToRemove = ccCArrayNew(8);

	_arrHandlerHelperData[CCTOUCHBEGAN] = CCTOUCHBEGAN;
	_arrHandlerHelperData[CCTOUCHMOVED] = CCTOUCHMOVED;
	_arrHandlerHelperData[CCTOUCHENDED] = CCTOUCHENDED;
	_arrHandlerHelperData[CCTOUCHCANCELLED] = CCTOUCHCANCELLED;
}

TouchDispatcher::~TouchDispatcher()
{
	CC_SAFE_RELEASE(_pOneHandlers);
	CC_SAFE_RELEASE(_pMultiHandlers);
	CC_SAFE_RELEASE(_pHandlersToAdd);

	ccCArrayFree(_pDelegateToRemove);
	_pDelegateToRemove = NULL;
}


void TouchDispatcher::addTargetedDelegate(TouchDelegate* pDelegate, int nPriority, bool bSwallowsTouches)
{
	TouchHandler* pHandler = TargetedTouchHandler::handlerWithDelegate(pDelegate, nPriority, bSwallowsTouches);
	if (!_bLocked)
	{
		forceAddHandler(pHandler, _pOneHandlers);
	}
	else
	{
		if (ccCArrayContainsValue(_pDelegateToRemove, pDelegate))
		{
			ccCArrayRemoveValue(_pDelegateToRemove, pDelegate);
			return;
		}

		_pHandlersToAdd->addObject(pHandler);
		_bToAdd = true;
	}
}


void TouchDispatcher::addStandardDelegate(TouchDelegate* pDelegate, int nPriority)
{
	TouchHandler* pHandler = StandardTouchHandler::handlerWithDelegate(pDelegate, nPriority);
	if (!_bLocked)
	{
		forceAddHandler(pHandler, _pMultiHandlers);
	}
	else
	{
		if (ccCArrayContainsValue(_pDelegateToRemove, pDelegate))
		{
			ccCArrayRemoveValue(_pDelegateToRemove, pDelegate);
			return;
		}

		_pHandlersToAdd->addObject(pHandler);
		_bToAdd = true;
	}
}
// 按优先级排序 且不能有重复的delegate
void TouchDispatcher::forceAddHandler(TouchHandler* pHandler, Array* pArray)
{
	unsigned int u = 0;

	Object* pObj = NULL;
	ARRAY_FOREACH(pArray, pObj)
	{
		TouchHandler* h = (TouchHandler*)pObj;
		if (h)
		{
			if (h->getPriority() < pHandler->getPriority())
			{
				++u;
			}
			
			if (h->getDelegate() == pHandler->getDelegate())
			{
				CCAssert(0, "");
				return;
			}
		}
	}

	pArray->insertObject(pHandler, u);
}


void TouchDispatcher::forceRemoveDelegate(TouchDelegate* pDelegate)
{
	TouchHandler* pHandler;

	Object* pObj = NULL;
	ARRAY_FOREACH(_pMultiHandlers, pObj)
	{
		pHandler = (TouchHandler*)pObj;
		if (pHandler && pHandler->getDelegate() == pDelegate)
		{
			_pMultiHandlers->removeObject(pHandler);
			break;
		}
	}

	ARRAY_FOREACH(_pOneHandlers, pObj)
	{
		pHandler = (TouchHandler*)pObj;
		if (pHandler && pHandler->getDelegate() == pDelegate)
		{
			_pOneHandlers->removeObject(pHandler);
			break;
		}
	}
}

void TouchDispatcher::forceRemoveAllDelegates(void)
{
	_pMultiHandlers->removeAllObjects();
	_pOneHandlers->removeAllObjects();
}

void TouchDispatcher::removeDelegate(TouchDelegate* pDelegate)
{
	if (pDelegate == NULL)
	{
		return;
	}

	if (!_bLocked)
	{
		forceRemoveDelegate(pDelegate);
	}
	else
	{
		TouchHandler* pHandler = findHandler(_pHandlersToAdd, pDelegate);
		if (pHandler)
		{
			_pHandlersToAdd->removeObject(pHandler);
			return;
		}

		ccCArrayAppendValue(_pDelegateToRemove, pDelegate);
		_bToRemove = true;
	}
}

void TouchDispatcher::removeAllDelegates(void)
{
	if (!_bLocked)
	{
		forceRemoveAllDelegates();
	}
	else
	{
		_bToQuit = true;
	}
}

void TouchDispatcher::rearrangeHandlers(Array* pArray)
{
	std::sort(pArray->data->arr, pArray->data->arr + pArray->data->num, less);
}

TouchHandler* TouchDispatcher::findHandler(TouchDelegate* pDelegate)
{
	Object* pObj = NULL;
	ARRAY_FOREACH(_pOneHandlers, pObj)
	{
		TouchHandler* pHandler = (TouchHandler*)pObj;
		if (pHandler->getDelegate() == pDelegate)
		{
			return pHandler;
		}
	}

	ARRAY_FOREACH(_pMultiHandlers, pObj)
	{
		TouchHandler* pHandler = (TouchHandler*)pObj;
		if (pHandler->getDelegate() == pDelegate)
		{
			return pHandler;
		}
	}

	return NULL;
}

TouchHandler* TouchDispatcher::findHandler(Array* pArray, TouchDelegate* pDelegate)
{
	CCAssert(pArray != NULL && pDelegate != NULL, "");

	Object* pObj = NULL;
	ARRAY_FOREACH(pArray, pObj)
	{
		TouchHandler* pHandle = (TouchHandler*)pObj;
		if (pHandle->getDelegate() == pDelegate)
		{
			return pHandle;
		}
	}

	return NULL;
}



bool TouchDispatcher::isDispatchEvents(void)
{
	return _bDispatchEvents;
}

void TouchDispatcher::setDispatchEvents(bool bDispatchEvents)
{
	_bDispatchEvents = bDispatchEvents;
}

void TouchDispatcher::setPriority(int nPriority, TouchDelegate* pDelegate)
{
	CCAssert(pDelegate != NULL, "");

	TouchHandler* handler = NULL;
	handler = this->findHandler(pDelegate);

	CCAssert(handler != NULL, "");

	if (handler->getPriority() != nPriority)
	{
		handler->setPriority(nPriority);
		this->rearrangeHandlers(_pOneHandlers);
		this->rearrangeHandlers(_pMultiHandlers);
	}
}


void TouchDispatcher::touchesBegan(Set* touches, TouchEvent* pEvent)
{
	if (_bDispatchEvents)
	{
		this->touches(touches, pEvent, CCTOUCHBEGAN);
	}
}

void TouchDispatcher::touchesMoved(Set* touches, TouchEvent* pEvent)
{
	if (_bDispatchEvents)
	{
		this->touches(touches, pEvent, CCTOUCHMOVED);
	}
}

void TouchDispatcher::touchesEnded(Set* touches, TouchEvent* pEvent)
{
	if (_bDispatchEvents)
	{
		this->touches(touches, pEvent, CCTOUCHENDED);
	}
}

void TouchDispatcher::touchesCancelled(Set* touches, TouchEvent* pEvent)
{
	if (_bDispatchEvents)
	{
		this->touches(touches, pEvent, CCTOUCHCANCELLED);
	}
}

void TouchDispatcher::touches(Set* pTouches, TouchEvent* pEvent, unsigned int uIndex)
{
	CCAssert(uIndex >= 0 && uIndex < 4, "");

	Set* pMutableTouches;
	_bLocked = true;

	// optimization to prevent a mutable copy when it is not necessary
	unsigned int uTargetedHandlersCount = _pOneHandlers->count();
	unsigned int uStandardHandlersCount = _pMultiHandlers->count();
	bool bNeedsMutableSet = (uTargetedHandlersCount && uStandardHandlersCount);

	pMutableTouches = (bNeedsMutableSet ? (Set*)pTouches->clone() : pTouches);

	const int type = _arrHandlerHelperData[uIndex];
	//
	// process the target handlers 1st
	//
	if (uTargetedHandlersCount > 0)
	{
		Touch* pTouch;
		SetIterator setIter;
		for (setIter = pTouches->begin(); setIter != pTouches->end(); ++setIter)
		{
			pTouch = (Touch*)(*setIter);

			TargetedTouchHandler* pHandler = NULL;
			Object* pObj = NULL;
			ARRAY_FOREACH(_pOneHandlers, pObj)
			{
				pHandler = (TargetedTouchHandler*)(pObj);

				if (!pHandler)
					break;

				bool bClaimed = false;
				if (uIndex == CCTOUCHBEGAN)
				{
					bClaimed = pHandler->getDelegate()->touchBegan(pTouch, pEvent);

					if (bClaimed)
					{
						pHandler->getClaimedTouches()->addObject(pTouch);
					}
				}
				else
				{
					if (pHandler->getClaimedTouches()->containsObject(pTouch))
					{
						// moved ended canceled
						bClaimed = true;

						switch (type)
						{
						case CCTOUCHMOVED:
							pHandler->getDelegate()->touchMoved(pTouch, pEvent);
							break;
						case CCTOUCHENDED:
							pHandler->getDelegate()->touchEnded(pTouch, pEvent);
							pHandler->getClaimedTouches()->removeObject(pTouch);
							break;
						case CCTOUCHCANCELLED:
							pHandler->getDelegate()->touchCancelled(pTouch, pEvent);
							pHandler->getClaimedTouches()->removeObject(pTouch);
							break;
						}
					}
				}

				if (bClaimed && pHandler->isSwallowsTouches())
				{
					if (bNeedsMutableSet)
					{
						pMutableTouches->removeObject(pTouch);
					}

					break;
				}
			}
		}
	}

	//
	// process standard handlers 2nd
	//
	if (uStandardHandlersCount > 0 && pMutableTouches->count() > 0)
	{
		StandardTouchHandler* pHandler = NULL;
		Object* pObj = NULL;
		ARRAY_FOREACH(_pMultiHandlers, pObj)
		{
			pHandler = (StandardTouchHandler*)(pObj);

			if (!pHandler)
			{
				break;
			}

			switch (type)
			{
			case CCTOUCHBEGAN:
				pHandler->getDelegate()->touchesBegan(pMutableTouches, pEvent);
				break;
			case CCTOUCHMOVED:
				pHandler->getDelegate()->touchesMoved(pMutableTouches, pEvent);
				break;
			case CCTOUCHENDED:
				pHandler->getDelegate()->touchesEnded(pMutableTouches, pEvent);
				break;
			case CCTOUCHCANCELLED:
				pHandler->getDelegate()->touchesCancelled(pMutableTouches, pEvent);
				break;
			}
		}
	}

	if (bNeedsMutableSet)
	{
		pMutableTouches->release();
	}

	//
	// Optimization. To prevent a [handlers copy] which is expensive
	// the add/removes/quit is done after the iterations
	//
	_bLocked = false;
	if (_bToRemove)
	{
		_bToRemove = false;
		for (unsigned int i = 0; i < _pDelegateToRemove->num; ++i)
		{
			forceRemoveDelegate((TouchDelegate*)_pDelegateToRemove->arr[i]);
		}
		ccCArrayRemoveAllValues(_pDelegateToRemove);
	}

	if (_bToAdd)
	{
		_bToAdd = false;

		TouchHandler* pHandler = NULL;
		Object* pObj = NULL;
		ARRAY_FOREACH(_pHandlersToAdd, pObj)
		{
			pHandler = (TouchHandler*)pObj;
			if (!pHandler)
				break;

			if (dynamic_cast<TargetedTouchHandler*>(pHandler) != NULL)
			{
				forceAddHandler(pHandler, _pOneHandlers);
			}
			else
			{
				forceAddHandler(pHandler, _pMultiHandlers);
			}
		}

		_pHandlersToAdd->removeAllObjects();
	}

	if (_bToQuit)
	{
		_bToQuit = false;
		forceRemoveAllDelegates();
	}
}





NS_CC_END