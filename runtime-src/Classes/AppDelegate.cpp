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
	CCDirector* pDirector = CCDirector::sharedDirector();
	EGLView* pEGLView = EGLView::sharedOpenGLView();

	pDirector->setOpenGLView(pEGLView);

	return true;
}


void AppDelegate::applicationDidEnterBackground() {
}

void AppDelegate::applicationWillEnterForeground() {
}

