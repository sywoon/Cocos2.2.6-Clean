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

	Size design = Director::sharedDirector()->getVisibleSize();
	Size size = this->getContentSize();
	CCLog("design:%f,%f size:%f,%f", design.width, design.height, size.width, size.height);

	DrawNode* node = DrawNode::create();
	node->setPosition(size.width/2, size.height/2);
	this->addChild(node);
	node->drawDot(ccp(0, 0), 10, ccc4f(1.0, 1.0, 0.0, 1.0));
	node->drawDot(ccp(size.width / 4, 0), 10, ccc4f(1.0, 0.0, 0.0, 1.0));
	node->drawDot(ccp(0, size.height / 4), 10, ccc4f(1.0, 0.0, 0.0, 1.0));
	node->drawSegment(ccp(0, 0), ccp(size.width / 4, size.height / 4), 3.0f, ccc4f(1.0, 1.0, 0.0, 1.0));

	const int count = 5;
	const int offx = -400;
	Point verts[5] = {
		{-200+offx, -200},
		{-200 + offx, 100},
		{0 + offx, 200},
		{200 + offx, 100},
		{200 + offx, -200},
	};
	ccColor4F fillColor = { 0.3, 0.0, 0.0, 1.0 };
	ccColor4F borderColor = { 0.3, 0.5, 0.0, 1.0 };
	node->drawPolygon(verts, count, fillColor, 10, borderColor);

	return this->initWithColor(ccc4(100, 100, 0, 255), ccc4(0, 100, 255, 255), ccp(0, 1));
}
