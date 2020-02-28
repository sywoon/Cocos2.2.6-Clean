#include "AppDelegate.h"
#include "Director.h"
#include "EGLView.h"


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

	return true;
}


void AppDelegate::applicationDidEnterBackground() {
}

void AppDelegate::applicationWillEnterForeground() {
}

