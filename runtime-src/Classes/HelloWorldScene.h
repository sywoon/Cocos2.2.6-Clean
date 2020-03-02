#pragma once

#include "cocos2d.h"


class HelloWorld : public cocos2d::LayerGradient
{
public:
	static HelloWorld* create(const cocos2d::ccColor4B& start, const cocos2d::ccColor4B& end);
	static cocos2d::Scene* scene();


protected:
	virtual bool init();
};



