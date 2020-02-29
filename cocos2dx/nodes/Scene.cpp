#include "Scene.h"
#include "cocoa/extend/PointExtend.h"
#include "Director.h"

NS_CC_BEGIN

Scene* Scene::create()
{
	Scene* pRet = new Scene();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}

Scene::Scene()
{
	_bIgnoreAnchorPointForPosition = true;
	setAnchorPoint(ccp(0.5f, 0.5f));
}

Scene::~Scene()
{
}

bool Scene::init()
{
	bool bRet = false;
	do
	{
		Director* pDirector;
		CC_BREAK_IF(!(pDirector = Director::sharedDirector()));
		this->setContentSize(pDirector->getWinSize());
		bRet = true;
	} while (0);
	return bRet;
}



NS_CC_END