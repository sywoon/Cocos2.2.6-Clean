#pragma once

#include <assert.h>

#ifndef MIN
#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#endif

#ifndef MAX
#define MAX(x,y) (((x) < (y)) ? (y) : (x))
#endif

// from "float.h"
#ifndef FLT_EPSILON
#define FLT_EPSILON     1.192092896e-07F        // smallest such that 1.0+FLT_EPSILON != 1.0
#endif

#ifndef DBL_EPSILON
#define DBL_EPSILON      2.2204460492503131e-016
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



#if CC_DISABLE_ASSERT > 0
#define CC_ASSERT(cond)
#else
#define CC_ASSERT(cond)    assert(cond)
#endif

#ifndef CCAssert
#if COCOS2D_DEBUG > 0
#define CCAssert(cond, msg) do {                              \
      if (!(cond)) {                                          \
        cocos2d::CCLog("Assert failed: %s", msg);             \
        CC_ASSERT(cond);                                      \
      }														  \
    } while (0)
#else
#define CCAssert(cond, msg) ((void)(cond))
#endif
#endif  // CCAssert

#define __CCLOGWITHFUNCTION(s, ...) \
    CCLog("%s : %s",__FUNCTION__, String::createWithFormat(s, ##__VA_ARGS__)->getCString())

// cocos2d debug
#if !defined(COCOS2D_DEBUG) || COCOS2D_DEBUG == 0
#define CCLOG(...)       do {} while (0)
#define CCLOGINFO(...)   do {} while (0)
#define CCLOGERROR(...)  do {} while (0)
#define CCLOGWARN(...)   do {} while (0)

#elif COCOS2D_DEBUG == 1
#define CCLOG(format, ...)      cocos2d::CCLog(format, ##__VA_ARGS__)
#define CCLOGERROR(format,...)  cocos2d::CCLog(format, ##__VA_ARGS__)
#define CCLOGINFO(format,...)   do {} while (0)
#define CCLOGWARN(...) __CCLOGWITHFUNCTION(__VA_ARGS__)

#elif COCOS2D_DEBUG > 1
#define CCLOG(format, ...)      cocos2d::CCLog(format, ##__VA_ARGS__)
#define CCLOGERROR(format,...)  cocos2d::CCLog(format, ##__VA_ARGS__)
#define CCLOGINFO(format,...)   cocos2d::CCLog(format, ##__VA_ARGS__)
#define CCLOGWARN(...) __CCLOGWITHFUNCTION(__VA_ARGS__)
#endif // COCOS2D_DEBUG


/*
 * only certain compiler support __attribute__((format))
 * formatPos - 1-based position of format string argument
 * argPos - 1-based position of first format-dependent argument
 */
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define CC_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#elif defined(__has_attribute)
#if __has_attribute(format)
#define CC_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#endif // __has_attribute(format)
#else
#define CC_FORMAT_PRINTF(formatPos, argPos)
#endif


#define CC_SAFE_DELETE(p)            do { if(p) { delete (p); (p) = 0; } } while(0)
#define CC_SAFE_DELETE_ARRAY(p)      do { if(p) { delete[] (p); (p) = 0; } } while(0)
#define CC_SAFE_FREE(p)              do { if(p) { free(p); (p) = 0; } } while(0)
#define CC_SAFE_RELEASE(p)           do { if(p) { (p)->release(); } } while(0)
#define CC_SAFE_RELEASE_NULL(p)      do { if(p) { (p)->release(); (p) = 0; } } while(0)
#define CC_SAFE_RETAIN(p)            do { if(p) { (p)->retain(); } } while(0)
#define CC_BREAK_IF(cond)            if(cond) break



#define CC_SWAP(x, y, type)    \
{    type temp = (x);        \
    x = y; y = temp;        \
}


//float between - 1 and 1
#define CCRANDOM_MINUS1_1() ((2.0f*((float)rand()/RAND_MAX))-1.0f)

//float between 0 and 1
#define CCRANDOM_0_1() ((float)rand()/RAND_MAX)


#define CC_DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) * 0.01745329252f) // PI / 180
#define CC_RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) * 57.29577951f) // PI * 180


#if !defined(COCOS2D_DEBUG) || COCOS2D_DEBUG == 0
#define CHECK_GL_ERROR_DEBUG()
#else
#define CHECK_GL_ERROR_DEBUG() \
    do { \
        GLenum __error = glGetError(); \
        if(__error) { \
            CCLog("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
        } \
    } while (false)
#endif


//=========================================
// opengl
//
#define CC_BLEND_SRC GL_ONE
#define CC_BLEND_DST GL_ONE_MINUS_SRC_ALPHA
