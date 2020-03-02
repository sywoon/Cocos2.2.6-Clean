#include "HelloWorldScene.h"




USING_NS_CC;


HelloWorld* HelloWorld::create(const cocos2d::ccColor4B& start, const cocos2d::ccColor4B& end)
{
	HelloWorld* pRet = new HelloWorld();
	if (pRet && pRet->initWithColor(start, end, ccp(0, 1)))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}

Scene* HelloWorld::scene()
{
	Scene* scene = Scene::create();

	HelloWorld* layer = HelloWorld::create(ccc4(100, 100, 0, 255), ccc4(0, 100, 255, 255));
	scene->addChild(layer);

	return scene;
}


bool HelloWorld::init()
{
	if (!LayerGradient::init())
		return false;

	Size size = Director::sharedDirector()->getVisibleSize();
	Point origin = Director::sharedDirector()->getVisibleOrigin();

	CCLog("visible origin:[%f,%f] size:[%f,%f]", origin.x, origin.y, size.width, size.height);

	return true;
}
