#include <windows.h>
#include <iostream>
#include "AppDelegate.h"

#define WIN32_LEAN_AND_MEAN
#define USE_WIN32_CONSOLE


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

	std::cout << "hello" << std::endl;
	return app.run();
}