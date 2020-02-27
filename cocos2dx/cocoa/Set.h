#pragma once

#include <set>
#include "Object.h"




NS_CC_BEGIN

typedef std::set<Object*>::iterator SetIterator;

class CC_DLL Set : public Object
{
	ACCEPT_VISITOR;

public:
	Set(void);
	Set(const Set& rSetObject);
	virtual ~Set(void);

	static Set* create();


public:
	int				count();

	void			addObject(Object* pObject);
	void			removeObject(Object* pObject);
	void			removeAllObjects();

	bool			containsObject(Object* pObject);

	SetIterator		begin();
	SetIterator		end();
	Object*			anyObject();  //����˳�� ȡ����һ������  �������������ĳ�����ܺ���

protected:
	virtual Object* cloneSelf();


private:
	std::set<Object*>* _pSet;
};

// end of data_structure group
/// @}

NS_CC_END








