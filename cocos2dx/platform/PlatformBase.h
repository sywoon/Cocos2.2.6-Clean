#pragma once

#include "ccMacros.h"


NS_CC_BEGIN

static const int kMaxLogLen = 16 * 1024;


void CC_DLL CCLog(const char* pszFormat, ...) CC_FORMAT_PRINTF(1, 2);

void CC_DLL CCMessageBox(const char* pszMsg, const char* pszTitle);


NS_CC_END