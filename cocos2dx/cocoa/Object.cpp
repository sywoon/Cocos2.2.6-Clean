#include "Object.h"
#include "platform/PlatformBase.h"
#include "AutoReleasePool.h"


NS_CC_BEGIN


Object::Object(void)
	: _uReference(1) // when the object is created, the reference count of it is 1
	, _uAutoReleaseCount(0)
{
	static unsigned int uObjectCount = 0;

	_uID = ++uObjectCount;
}

Object::~Object(void)
{
	// if the object is managed, we should remove it
	// from pool manager
	if (_uAutoReleaseCount > 0)
	{
		PoolManager::sharedPoolManager()->removeObject(this);
	}
}

void Object::release(void)
{
	CCAssert(_uReference > 0, "reference count should greater than 0");
	--_uReference;

	if (_uReference == 0)
	{
		delete this;
	}
}

void Object::retain(void)
{
	CCAssert(_uReference > 0, "reference count should greater than 0");

	++_uReference;
}

Object* Object::autorelease(void)
{
	PoolManager::sharedPoolManager()->addObject(this);
	return this;
}

bool Object::isSingleReference(void) const
{
	return _uReference == 1;
}

unsigned int Object::retainCount(void) const
{
	return _uReference;
}

Object* Object::clone(void)
{
	return cloneSelf();
}

Object* Object::cloneSelf()
{
	CCAssert(0, "not implement");
	return NULL;
}

bool Object::isEqual(const Object* pObject)
{
	return this == pObject;
}


NS_CC_END