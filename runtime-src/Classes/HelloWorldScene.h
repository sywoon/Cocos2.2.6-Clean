#pragma once

#include "cocos2d.h"


class HelloWorld : public cocos2d::LayerGradient
{
public:
	CREATE_FUNC(HelloWorld);

	static cocos2d::Scene* scene();

public:
	virtual void draw(void);

protected:
	virtual bool init();
};



