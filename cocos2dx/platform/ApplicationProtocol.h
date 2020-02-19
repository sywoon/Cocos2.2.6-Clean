#pragma once

#include "ccMacros.h"

NS_CC_BEGIN

enum TargetPlatform
{
	kTargetWindows,
	kTargetLinux,
	kTargetMacOS,
	kTargetAndroid,
	kTargetIos,
};

enum LanguageType
{
	kLanguageEnglish = 0,
	kLanguageChinese,
	kLanguageFrench,
	kLanguageItalian,
	kLanguageGerman,
	kLanguageSpanish,
	kLanguageDutch,
	kLanguageRussian,
	kLanguageKorean,
	kLanguageJapanese,
	kLanguageHungarian,
	kLanguagePortuguese,
	kLanguageArabic
};


class CC_DLL ApplicationProtocol
{
public:
	virtual ~ApplicationProtocol() {}


	virtual bool applicationDidFinishLaunching() = 0;
	virtual void applicationDidEnterBackground() = 0;
	virtual void applicationWillEnterForeground() = 0;

	virtual void setAnimationInterval(double interval) = 0;

	virtual LanguageType getCurrentLanguage() = 0;
	virtual TargetPlatform getTargetPlatform() = 0;
};




NS_CC_END






