#pragma once
#include "ccMacros.h"
#include "cocoa/Object.h"


NS_CC_BEGIN


class TouchDelegate;
class CC_DLL  TouchHandler : public Object
{
public:
	TouchHandler(void);
	virtual ~TouchHandler(void);

	static TouchHandler* handlerWithDelegate(TouchDelegate* pDelegate, int nPriority);

	
public:
	TouchDelegate*	getDelegate();
	void			setDelegate(TouchDelegate* pDelegate);

	int				getPriority(void);
	void			setPriority(int nPriority);

private:
	void			releaseDelegate();

protected:
	virtual bool	initWithDelegate(TouchDelegate* pDelegate, int nPriority);


protected:
	TouchDelegate*	_pDelegate;
	int				_nPriority;

};


class CC_DLL  StandardTouchHandler : public TouchHandler
{
public:
	static StandardTouchHandler* handlerWithDelegate(TouchDelegate* pDelegate, int nPriority);
};


class CC_DLL  TargetedTouchHandler : public TouchHandler
{
public:
	TargetedTouchHandler(void);
	~TargetedTouchHandler(void);

	static TargetedTouchHandler* handlerWithDelegate(TouchDelegate* pDelegate, int nPriority, bool bSwallow);

public:
	bool		isSwallowsTouches(void);
	void		setSwallowsTouches(bool bSwallowsTouches);

	Set*		getClaimedTouches(void);

	bool		initWithDelegate(TouchDelegate* pDelegate, int nPriority, bool bSwallow);


protected:
	bool	_bSwallowsTouches;
	Set*	_pClaimedTouches;
};


NS_CC_END
