#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::scene()
{
	Scene* scene = Scene::create();

	HelloWorld* layer = HelloWorld::create();
	scene->addChild(layer);

	return scene;
}


bool HelloWorld::init()
{
	if (!Layer::init())
		return false;

	Size size = Director::sharedDirector()->getVisibleSize();
	Point origin = Director::sharedDirector()->getVisibleOrigin();

	CCLog("visible origin:[%f,%f] size:[%f,%f]", origin.x, origin.y, size.width, size.height);

	return true;
}
