#pragma once

#include "ccMacros.h"
#include "DataVisitor.h"


NS_CC_BEGIN

class CC_DLL Object
{
public:
	Object(void);
	virtual ~Object(void);

public:
	void			retain(void);
	void			release(void);
	Object*			autorelease(void);

	bool			isSingleReference(void) const;
	unsigned int	retainCount(void) const;

	Object*			clone(void);

public:
	virtual bool	isEqual(const Object* pObject);
	virtual void	acceptVisitor(DataVisitor& visitor);

	virtual void	update(float dt) { CC_UNUSED_PARAM(dt); };
	
protected:
	virtual Object* cloneSelf();


public:
	unsigned int        _uID;
	unsigned int        _uReference;
	unsigned int        _uAutoReleaseCount;
};


class Node;
class TouchEvent;

typedef void (Object::* SEL_SCHEDULE)(float);
typedef void (Object::* SEL_CallFunc)();
typedef void (Object::* SEL_CallFuncN)(Node*);
typedef void (Object::* SEL_CallFuncND)(Node*, void*);
typedef void (Object::* SEL_CallFuncO)(Object*);
typedef void (Object::* SEL_MenuHandler)(Object*);
typedef void (Object::* SEL_EventHandler)(TouchEvent*);
typedef int  (Object::* SEL_Compare)(Object*);

#define schedule_selector(_SELECTOR) (SEL_SCHEDULE)(&_SELECTOR)
#define callfunc_selector(_SELECTOR) (SEL_CallFunc)(&_SELECTOR)
#define callfuncN_selector(_SELECTOR) (SEL_CallFuncN)(&_SELECTOR)
#define callfuncND_selector(_SELECTOR) (SEL_CallFuncND)(&_SELECTOR)
#define callfuncO_selector(_SELECTOR) (SEL_CallFuncO)(&_SELECTOR)
#define menu_selector(_SELECTOR) (SEL_MenuHandler)(&_SELECTOR)
#define event_selector(_SELECTOR) (SEL_EventHandler)(&_SELECTOR)
#define compare_selector(_SELECTOR) (SEL_Compare)(&_SELECTOR)



NS_CC_END