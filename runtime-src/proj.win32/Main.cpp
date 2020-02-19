#include <windows.h>
#include <iostream>
#include "cocos2d.h"

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

	std::cout << "hello" << addTest(1, 2) << std::endl;
	return 0;
}