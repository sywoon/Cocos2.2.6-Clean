#include <windows.h>
#include <iostream>
#include "AppDelegate.h"
#include "EGLView.h"

#define WIN32_LEAN_AND_MEAN
#define USE_WIN32_CONSOLE

USING_NS_CC;



int APIENTRY WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine,
	int nCmdShow
)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	

#ifdef USE_WIN32_CONSOLE
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
#endif

	
	AppDelegate app;

	EGLView* eglView = EGLView::sharedOpenGLView();
	eglView->setViewName("HelloCpp");
	eglView->setFrameSize(2048, 1536);
	// The resolution of ipad3 is very large. In general, PC's resolution is smaller than it.
	// So we need to invoke 'setFrameZoomFactor'(only valid on desktop(win32, mac, linux)) to make the window smaller.
	eglView->setFrameZoomFactor(0.4f);

	std::cout << "hello" << std::endl;
	return app.run();
}

