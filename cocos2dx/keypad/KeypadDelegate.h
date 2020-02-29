#pragma once
#include "ccMacros.h"
#include "cocoa/Object.h"


NS_CC_BEGIN

class CC_DLL KeypadDelegate
{
public:
	// The back key clicked
	virtual void keyBackClicked() {}

	// The menu key clicked. only available on wophone & android
	virtual void keyMenuClicked() {};
};



NS_CC_END