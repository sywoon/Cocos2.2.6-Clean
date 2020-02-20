#pragma once

#include <assert.h>

#ifndef MIN
#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#endif

#ifndef MAX
#define MAX(x,y) (((x) < (y)) ? (y) : (x))
#endif

#ifndef FLT_EPSILON
#define FLT_EPSILON     1.192092896e-07F
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


