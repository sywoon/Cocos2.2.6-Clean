#pragma once

#include "platform/ApplicationProtocol.h"
#include <windows.h>



NS_CC_BEGIN

class CC_DLL Application : public ApplicationProtocol
{
public:
	Application();
	virtual ~Application();

	static Application*	sharedApplication();

public:
	virtual void			setAnimationInterval(double interval);

	virtual LanguageType	getCurrentLanguage();
	virtual TargetPlatform	getTargetPlatform();

public:
	virtual int				run();



protected:
	HINSTANCE				_hInstance;
	LARGE_INTEGER			_nAnimationInterval;

	static Application*	s_pSharedApplication;
};


NS_CC_END