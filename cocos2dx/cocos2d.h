#pragma once

#include "ccMacros.h"


/////////////////////////
// platform
//
#include "platform/ApplicationProtocol.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "platform/win32/Application.h"
#include "platform/win32/EGLView.h"
#include "platform/win32/Accelerometer.h"
#endif




/////////////////////////
// cocoa
//
#include "cocoa/data_support/ccCArray.h"
#include "cocoa/data_support/uthash.h"
#include "cocoa/extend/AffineExtend.h"
#include "cocoa/extend/PointExtend.h"
#include "cocoa/AffineTransform.h"
#include "cocoa/Array.h"
#include "cocoa/AutoReleasePool.h"
#include "cocoa/Bool.h"
#include "cocoa/DataVisitor.h"
#include "cocoa/Dictionary.h"
#include "cocoa/Double.h"
#include "cocoa/Float.h"
#include "cocoa/Integer.h"
#include "cocoa/String.h"
#include "cocoa/Set.h"
#include "cocoa/Object.h"
#include "cocoa/Geometry.h"

/////////////////////////
// kazmath
//
#include "kazmath/GL/matrix.h"
#include "kazmath/kazmath.h"





NS_CC_BEGIN

CC_DLL const char* cocos2dVersion();

NS_CC_END