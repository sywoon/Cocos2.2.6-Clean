#pragma once

#include <assert.h>

#if CC_DISABLE_ASSERT > 0
#define CC_ASSERT(cond)
#else
#define CC_ASSERT(cond)    assert(cond)
#endif

#ifdef _USRDLL
#define CC_DLL _declspec(dllexport)
#else
#define CC_DLL _declspec(dllimport)
#endif

// namespace cocos2d {}
#ifdef __cplusplus
#define NS_CC_BEGIN                     namespace cocos2d {
#define NS_CC_END                       }
#define USING_NS_CC                     using namespace cocos2d;
#else
#define NS_CC_BEGIN 
#define NS_CC_END 
#define USING_NS_CC 
#endif 


