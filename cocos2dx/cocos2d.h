#pragma once

#include "ccMacros.h"


#include "platform/ApplicationProtocol.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "platform/win32/Application.h"
#endif






NS_CC_BEGIN

CC_DLL const char* cocos2dVersion();

NS_CC_END