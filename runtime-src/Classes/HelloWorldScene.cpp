#include "HelloWorldScene.h"




USING_NS_CC;


Scene* HelloWorld::scene()
{
	Scene* scene = Scene::create();

	HelloWorld* layer = HelloWorld::create();
	scene->addChild(layer);

	Size size = Director::sharedDirector()->getVisibleSize();
	Point origin = Director::sharedDirector()->getVisibleOrigin();

	std::string writePath = FileUtils::sharedFileUtils()->getWritablePath();

	CCLog("visible origin:[%.2f,%.2f] size:[%.2f,%.2f] \npath:%s", origin.x, origin.y, size.width, size.height,
					writePath.c_str());

	return scene;
}


bool HelloWorld::init()
{
	if (!LayerGradient::init())
		return false;

	DrawNode* node = DrawNode::create();
	this->addChild(node, 100);
	//node->drawDot(ccp(100, 100), 10, ccc4f(1.0, 1.0, 0.0, 1.0));
	node->drawSegment(ccp(0, 0), ccp(100, 100), 3.0f, ccc4f(1.0, 1.0, 0.0, 1.0));

	return true;
	//return this->initWithColor(ccc4(100, 100, 0, 255), ccc4(0, 100, 255, 255), ccp(0, 1));
}
