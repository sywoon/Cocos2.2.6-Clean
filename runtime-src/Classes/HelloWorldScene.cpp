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

	Sprite* sprite = Sprite::create("HelloWorld.png");
	sprite->setPosition(size.width / 2, size.height / 2);
	//sprite->setContentSize(SizeMake(300, 300));  改变位置 大小不变
	sprite->setScale(1.5);
	this->addChild(sprite);

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
	ccColor4F fillColor = { 0.3f, 0.0f, 0.0f, 1.0f };
	ccColor4F borderColor = { 0.3f, 0.5f, 0.0f, 1.0f };
	node->drawPolygon(verts, count, fillColor, 10, borderColor);

	return this->initWithColor(ccc4(100, 100, 0, 255), ccc4(0, 100, 255, 255), ccp(0, 1));
}

void HelloWorld::draw(void)
{
	LayerGradient::draw();

	ccPointSize(30);
	ccDrawColor4F(0.3f, 0.0f, 0.0f, 1.0f);

	Size design = Director::sharedDirector()->getVisibleSize();
	Point pos(design.width/4, design.height/4);
	ccDrawPoint(pos);

	ccDrawLine(Point(0, 0), Point(300, 300));

	ccDrawCircle(Point(500, 500), 100, 0, 10, false);
	Point vertices[] = { ccp(0,0), ccp(50,50), ccp(100,50), ccp(100,100), ccp(50,100) };
	ccDrawPoly(vertices, 5, false);

	ccLineWidth(1);
	Point filledVertices[] = { ccp(0,120), ccp(50,120), ccp(50,170), ccp(25,200), ccp(0,170) };
	ccDrawSolidPoly(filledVertices, 5, ccc4f(0.5f, 0.5f, 1, 1));

	ccDrawQuadBezier(Point(0, 400), Point(400, 400), Point(400, 700), 50);

	ccDrawCubicBezier(Point(400, 400), ccp(400 + 30, 400 + 50), ccp(400 + 60, 400 - 50), ccp(500, 300), 100);

	//CHECK_GL_ERROR_DEBUG();
}



