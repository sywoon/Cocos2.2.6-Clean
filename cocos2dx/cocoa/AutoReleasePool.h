#pragma once

#include "ccMacros.h"
#include "Object.h"

NS_CC_BEGIN

class Array;
class CC_DLL AutoreleasePool : public Object
{
public:
	AutoreleasePool(void);
	~AutoreleasePool(void);

	void addObject(Object* pObject);
	void removeObject(Object* pObject);

	void clear();

private:
	Array* _pManagedObjectArray;
};



class CC_DLL PoolManager
{
public:
	PoolManager();
	~PoolManager();

	static PoolManager*		sharedPoolManager();
	static void				purgePoolManager();

	friend class AutoreleasePool;

public:
	void		finalize();
	void		push();
	void		pop();

	void		removeObject(Object* pObject);
	void		addObject(Object* pObject);

private:
	AutoreleasePool* getCurReleasePool();

private:
	Array*				_pReleasePoolStack;
	AutoreleasePool*	_pCurReleasePool;
};





NS_CC_END