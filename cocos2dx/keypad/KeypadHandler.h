#pragma once
#include "ccMacros.h"
#include "cocoa/Object.h"


NS_CC_BEGIN


class KeypadDelegate;
class CC_DLL KeypadHandler : public Object
{
public:
	KeypadHandler();
	virtual ~KeypadHandler(void);

	// 用一个delegate创建一个handler
	static KeypadHandler* handlerWithDelegate(KeypadDelegate* pDelegate);

public:
	KeypadDelegate*		getDelegate();
	void				setDelegate(KeypadDelegate* pDelegate);

protected:
	virtual bool		initWithDelegate(KeypadDelegate* pDelegate);

private:
	void				releaseDelegate();


protected:
	KeypadDelegate*		_pDelegate;
};


NS_CC_END