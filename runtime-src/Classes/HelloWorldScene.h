#pragma once

#include "cocos2d.h"

class HelloWorld : public cocos2d::Layer
{
public:
	CREATE_FUNC(HelloWorld);
	static cocos2d::Scene* scene();


protected:
	virtual bool init();
};



