#include "Set.h"


NS_CC_BEGIN

Set::Set(void)
{
	_pSet = new std::set<Object*>;
}

Set::Set(const Set& rSetObject)
{
	_pSet = new std::set<Object*>(*rSetObject._pSet);

	// call retain of members
	SetIterator iter;
	for (iter = _pSet->begin(); iter != _pSet->end(); ++iter)
	{
		if (!(*iter))
		{
			break;
		}

		(*iter)->retain();
	}
}

Set::~Set(void)
{
	removeAllObjects();
	CC_SAFE_DELETE(_pSet);
}


Set* Set::create()
{
	Set* pRet = new Set();

	if (pRet != NULL)
	{
		pRet->autorelease();
	}

	return pRet;
}

Object* Set::cloneSelf(void)
{
	Set* pSet = new Set(*this);

	return pSet;
}


int Set::count(void)
{
	return (int)_pSet->size();
}

void Set::addObject(Object* pObject)
{
	CC_SAFE_RETAIN(pObject);
	_pSet->insert(pObject);
}

void Set::removeObject(Object* pObject)
{
	_pSet->erase(pObject);
	CC_SAFE_RELEASE(pObject);
}

void Set::removeAllObjects()
{
	SetIterator it;
	for (it = _pSet->begin(); it != _pSet->end(); ++it)
	{
		if (!(*it))
		{
			break;
		}

		(*it)->release();
	}
	_pSet->clear();
}

bool Set::containsObject(Object* pObject)
{
	return _pSet->find(pObject) != _pSet->end();
}

SetIterator Set::begin(void)
{
	return _pSet->begin();
}

SetIterator Set::end(void)
{
	return _pSet->end();
}

Object* Set::anyObject()
{
	if (!_pSet || _pSet->empty())
	{
		return NULL;
	}

	SetIterator it;

	for (it = _pSet->begin(); it != _pSet->end(); ++it)
	{
		if (*it)
		{
			return (*it);
		}
	}

	return 0;
}

NS_CC_END
