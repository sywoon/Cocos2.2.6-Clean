#include "Application.h"
#include "ccMacros.h"



NS_CC_BEGIN

Application* Application::s_pSharedApplication = NULL;

Application* Application::sharedApplication()
{
	CC_ASSERT(s_pSharedApplication);
	return s_pSharedApplication;
}

Application::Application()
	: _hInstance(NULL)
{
	_hInstance = GetModuleHandle(NULL);
	_nAnimationInterval.QuadPart = 0;

	CC_ASSERT(!s_pSharedApplication);
	s_pSharedApplication = this;
}

Application::~Application()
{
	CC_ASSERT(this == s_pSharedApplication);
	s_pSharedApplication = NULL;
}

void Application::setAnimationInterval(double interval)
{
	LARGE_INTEGER nFreq;
	QueryPerformanceFrequency(&nFreq);
	_nAnimationInterval.QuadPart = (LONGLONG)(interval * nFreq.QuadPart);
}


LanguageType Application::getCurrentLanguage()
{
	LanguageType ret = kLanguageEnglish;

	LCID localeID = GetUserDefaultLCID();
	unsigned short primaryLanguageID = localeID & 0xFF;

	switch (primaryLanguageID)
	{
	case LANG_CHINESE:
		ret = kLanguageChinese;
		break;
	case LANG_ENGLISH:
		ret = kLanguageEnglish;
		break;
	case LANG_FRENCH:
		ret = kLanguageFrench;
		break;
	case LANG_ITALIAN:
		ret = kLanguageItalian;
		break;
	case LANG_GERMAN:
		ret = kLanguageGerman;
		break;
	case LANG_SPANISH:
		ret = kLanguageSpanish;
		break;
	case LANG_DUTCH:
		ret = kLanguageDutch;
		break;
	case LANG_RUSSIAN:
		ret = kLanguageRussian;
		break;
	case LANG_KOREAN:
		ret = kLanguageKorean;
		break;
	case LANG_JAPANESE:
		ret = kLanguageJapanese;
		break;
	case LANG_HUNGARIAN:
		ret = kLanguageHungarian;
		break;
	case LANG_PORTUGUESE:
		ret = kLanguagePortuguese;
		break;
	case LANG_ARABIC:
		ret = kLanguageArabic;
		break;
	}

	return ret;
}

TargetPlatform Application::getTargetPlatform()
{
	return kTargetWindows;
}



int Application::run()
{
	MSG msg;
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nLast;
	LARGE_INTEGER nNow;

	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nLast);

	// Initialize instance and cocos2d.
	if (!applicationDidFinishLaunching())
	{
		return 0;
	}

	while (1)
	{
		if (!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			QueryPerformanceCounter(&nNow);

			if (nNow.QuadPart - nLast.QuadPart > _nAnimationInterval.QuadPart)
			{
				nLast.QuadPart = nNow.QuadPart;
				//CCDirector::sharedDirector()->mainLoop();
			}
			else
			{
				Sleep(0);
			}
			continue;
		}

		if (WM_QUIT == msg.message)
			break;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}


NS_CC_END


