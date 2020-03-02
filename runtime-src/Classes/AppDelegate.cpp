#include "AppDelegate.h"
#include "Director.h"
#include "EGLView.h"
#include "HelloWorldScene.h"


using namespace std;

USING_NS_CC


AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate()
{
}



bool AppDelegate::applicationDidFinishLaunching()
{
	Director* pDirector = Director::sharedDirector();
	EGLView* pEGLView = EGLView::sharedOpenGLView();

	pDirector->setOpenGLView(pEGLView);
	pEGLView->setDesignResolutionSize(960, 640, kResolutionNoBorder);

	Scene* pScene = HelloWorld::scene();
	pDirector->runWithScene(pScene);

	return true;
}


void AppDelegate::applicationDidEnterBackground() {
	Director::sharedDirector()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground() {
	Director::sharedDirector()->startAnimation();
}


