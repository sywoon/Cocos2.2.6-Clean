#include "AutoReleasePool.h"
#include "platform/PlatformBase.h"
#include "Array.h"

NS_CC_BEGIN


AutoreleasePool::AutoreleasePool(void)
{
	_pManagedObjectArray = new Array();
	_pManagedObjectArray->init();
}

AutoreleasePool::~AutoreleasePool(void)
{
	CC_SAFE_DELETE(_pManagedObjectArray);
}

void AutoreleasePool::addObject(Object* pObject)
{
	_pManagedObjectArray->addObject(pObject);

	CCAssert(pObject->_uReference > 1, "reference count should be greater than 1");
	++(pObject->_uAutoReleaseCount);
	pObject->release(); // no ref count, in this case autorelease pool added.
}

void AutoreleasePool::removeObject(Object* pObject)
{
	for (unsigned int i = 0; i < pObject->_uAutoReleaseCount; ++i)
	{
		_pManagedObjectArray->removeObject(pObject, false);
	}
	pObject->_uAutoReleaseCount = 0;
}

void AutoreleasePool::clear()
{
	if (_pManagedObjectArray->count() > 0)
	{
		Object* pObj = NULL;
		ARRAY_FOREACH_REVERSE(_pManagedObjectArray, pObj)
		{
			if (!pObj)
				break;

			pObj->_uAutoReleaseCount = 0;
		}

		_pManagedObjectArray->removeAllObjects();
	}
}




//--------------------------------------------------------------------
//
// PoolManager
//
//--------------------------------------------------------------------
static PoolManager* s_pPoolManager = NULL;

PoolManager* PoolManager::sharedPoolManager()
{
	if (s_pPoolManager == NULL)
	{
		s_pPoolManager = new PoolManager();
	}
	return s_pPoolManager;
}

void PoolManager::purgePoolManager()
{
	CC_SAFE_DELETE(s_pPoolManager);
}

PoolManager::PoolManager()
{
	_pReleasePoolStack = new Array();
	_pReleasePoolStack->init();
	_pCurReleasePool = 0;
}

PoolManager::~PoolManager()
{
	finalize();

	_pCurReleasePool = 0;
	CC_SAFE_DELETE(_pReleasePoolStack);
}

void PoolManager::finalize()
{
	if (_pReleasePoolStack->count() > 0)
	{
		//AutoreleasePool* pReleasePool;
		Object* pObj = NULL;
		ARRAY_FOREACH(_pReleasePoolStack, pObj)
		{
			if (!pObj)
				break;
			AutoreleasePool* pPool = (AutoreleasePool*)pObj;
			pPool->clear();
		}
	}
}

void PoolManager::push()
{
	AutoreleasePool* pPool = new AutoreleasePool();       //ref = 1
	_pCurReleasePool = pPool;

	_pReleasePoolStack->addObject(pPool);                   //ref = 2

	pPool->release();                                       //ref = 1
}

void PoolManager::pop()
{
	if (!_pCurReleasePool)
	{
		return;
	}

	int nCount = _pReleasePoolStack->count();

	_pCurReleasePool->clear();

	if (nCount > 1)
	{
		_pReleasePoolStack->removeObjectAtIndex(nCount - 1);

		//         if(nCount > 1)
		//         {
		//             _pCurReleasePool = _pReleasePoolStack->objectAtIndex(nCount - 2);
		//             return;
		//         }
		_pCurReleasePool = (AutoreleasePool*)_pReleasePoolStack->objectAtIndex(nCount - 2);
	}

	/*_pCurReleasePool = NULL;*/
}

void PoolManager::removeObject(Object * pObject)
{
	CCAssert(_pCurReleasePool, "current auto release pool should not be null");

	_pCurReleasePool->removeObject(pObject);
}

void PoolManager::addObject(Object * pObject)
{
	getCurReleasePool()->addObject(pObject);
}


AutoreleasePool* PoolManager::getCurReleasePool()
{
	if (!_pCurReleasePool)
	{
		push();
	}

	CCAssert(_pCurReleasePool, "current auto release pool should not be null");

	return _pCurReleasePool;
}


NS_CC_END
