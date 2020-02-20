#include "Array.h"
#include "platform/PlatformBase.h"
#include <math.h>


NS_CC_BEGIN


Array::Array()
	: data(NULL)
{
}

Array::Array(unsigned int capacity)
	: data(NULL)
{
	initWithCapacity(capacity);
}

Array* Array::create()
{
	Array* pArray = new Array();

	if (pArray && pArray->init())
	{
		pArray->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pArray);
	}

	return pArray;
}

Array* Array::createWithCapacity(unsigned int capacity)
{
	Array* pArray = new Array();

	if (pArray && pArray->initWithCapacity(capacity))
	{
		pArray->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pArray);
	}

	return pArray;
}

Array* Array::createWithObject(Object* pObject)
{
	Array* pArray = new Array();

	if (pArray && pArray->initWithObject(pObject))
	{
		pArray->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pArray);
	}

	return pArray;
}

Array* Array::create(Object* pObject, ...)
{
	va_list args;
	va_start(args, pObject);

	Array* pArray = create();
	if (pArray && pObject)
	{
		pArray->addObject(pObject);
		Object* i = va_arg(args, Object*);
		while (i)
		{
			pArray->addObject(i);
			i = va_arg(args, Object*);
		}
	}
	else
	{
		CC_SAFE_DELETE(pArray);
	}

	va_end(args);

	return pArray;
}

Array* Array::createWithArray(Array* otherArray)
{
	Array* pRet = (Array*)otherArray->clone();
	pRet->autorelease();
	return pRet;
}


bool Array::init()
{
	return initWithCapacity(1);
}

bool Array::initWithCapacity(unsigned int capacity)
{
	ccArrayFree(data);
	data = ccArrayNew(capacity);
	return true;
}

bool Array::initWithObject(Object* pObject)
{
	ccArrayFree(data);
	bool bRet = initWithCapacity(1);
	if (bRet)
	{
		addObject(pObject);
	}
	return bRet;
}

bool Array::initWithObjects(Object* pObject, ...)
{
	ccArrayFree(data);
	bool bRet = false;
	do
	{
		CC_BREAK_IF(pObject == NULL);

		va_list args;
		va_start(args, pObject);

		if (pObject)
		{
			this->addObject(pObject);
			Object* i = va_arg(args, Object*);
			while (i)
			{
				this->addObject(i);
				i = va_arg(args, Object*);
			}
			bRet = true;
		}
		va_end(args);

	} while (false);

	return bRet;
}


bool Array::initWithArray(Array* otherArray)
{
	ccArrayFree(data);
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!initWithCapacity(otherArray->data->num));

		addObjectsFromArray(otherArray);
		bRet = true;
	} while (0);

	return bRet;
}

unsigned int Array::count() const
{
	return data->num;
}

unsigned int Array::capacity() const
{
	return data->max;
}

unsigned int Array::indexOfObject(Object* object) const
{
	return ccArrayGetIndexOfObject(data, object);
}

Object* Array::objectAtIndex(unsigned int index)
{
	CCAssert(index < data->num, "index out of range in objectAtIndex()");

	return data->arr[index];
}

Object* Array::lastObject()
{
	if (data->num > 0)
		return data->arr[data->num - 1];

	return NULL;
}

Object* Array::randomObject()
{
	if (data->num == 0)
	{
		return NULL;
	}

	float r = CCRANDOM_0_1();

	if (r == 1) // to prevent from accessing data-arr[data->num], out of range.
	{
		r = 0;
	}

	return data->arr[(int)(data->num * r)];
}

bool Array::containsObject(Object* object) const
{
	return ccArrayContainsObject(data, object);
}

bool Array::isEqualToArray(Array* otherArray)
{
	for (unsigned int i = 0; i < this->count(); i++)
	{
		if (!this->objectAtIndex(i)->isEqual(otherArray->objectAtIndex(i)))
		{
			return false;
		}
	}
	return true;
}

void Array::addObject(Object* object)
{
	ccArrayAppendObjectWithResize(data, object);
}

void Array::addObjectsFromArray(Array* otherArray)
{
	ccArrayAppendArrayWithResize(data, otherArray->data);
}

void Array::insertObject(Object* object, unsigned int index)
{
	ccArrayInsertObjectAtIndex(data, object, index);
}

void Array::removeLastObject(bool bReleaseObj)
{
	CCAssert(data->num, "no objects added");
	ccArrayRemoveObjectAtIndex(data, data->num - 1, bReleaseObj);
}

void Array::removeObject(Object* object, bool bReleaseObj/* = true*/)
{
	ccArrayRemoveObject(data, object, bReleaseObj);
}

void Array::removeObjectAtIndex(unsigned int index, bool bReleaseObj)
{
	ccArrayRemoveObjectAtIndex(data, index, bReleaseObj);
}

void Array::removeObjectsInArray(Array* otherArray)
{
	ccArrayRemoveArray(data, otherArray->data);
}

void Array::removeAllObjects()
{
	ccArrayRemoveAllObjects(data);
}

void Array::fastRemoveObjectAtIndex(unsigned int index)
{
	ccArrayFastRemoveObjectAtIndex(data, index);
}

void Array::fastRemoveObject(Object* object)
{
	ccArrayFastRemoveObject(data, object);
}

void Array::exchangeObject(Object* object1, Object* object2)
{
	unsigned int index1 = ccArrayGetIndexOfObject(data, object1);
	if (index1 == UINT_MAX)
	{
		return;
	}

	unsigned int index2 = ccArrayGetIndexOfObject(data, object2);
	if (index2 == UINT_MAX)
	{
		return;
	}

	ccArraySwapObjectsAtIndexes(data, index1, index2);
}

void Array::exchangeObjectAtIndex(unsigned int index1, unsigned int index2)
{
	ccArraySwapObjectsAtIndexes(data, index1, index2);
}

void Array::replaceObjectAtIndex(unsigned int index, Object* pObject, bool bReleaseObject/* = true*/)
{
	ccArrayInsertObjectAtIndex(data, pObject, index);
	ccArrayRemoveObjectAtIndex(data, index + 1);
}

void Array::reverseObjects()
{
	if (data->num > 1)
	{
		// floorf(), since in the case of an even number, the number of swaps stays the same
		int count = (int)floorf(data->num / 2.f);
		unsigned int maxIndex = data->num - 1;

		for (int i = 0; i < count; i++)
		{
			ccArraySwapObjectsAtIndexes(data, i, maxIndex);
			maxIndex--;
		}
	}
}

void Array::reduceMemoryFootprint()
{
	ccArrayShrink(data);
}

Array::~Array()
{
	ccArrayFree(data);
}

Object* Array::cloneSelf()
{
	Array* pArray = new Array();
	pArray->initWithCapacity(this->data->num > 0 ? this->data->num : 1);

	Object* pObj = NULL;
	Object* pTmpObj = NULL;
	ARRAY_FOREACH(this, pObj)
	{
		pTmpObj = pObj->clone();
		pArray->addObject(pTmpObj);
		pTmpObj->release();
	}
	return pArray;
}




NS_CC_END
