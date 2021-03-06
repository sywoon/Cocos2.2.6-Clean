#pragma once

#include "ccMacros.h"
#include "ccType.h"



/////////////////////////
// platform
//
#include "platform/ApplicationProtocol.h"
#include "platform/FileUtils.h"

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


/////////////////////////
// Node
//
#include "nodes/base_nodes/Node.h"
#include "nodes/draw_nodes/DrawNode.h"
#include "nodes/draw_nodes/DrawNodePrimitives.h"


/////////////////////////
// layer
//
#include "nodes/layer/Layer.h"
#include "nodes/layer/LayerRBGA.h"
#include "nodes/layer/LayerColor.h"
#include "nodes/layer/LayerGradient.h"
#include "nodes/layer/LayerMultiplex.h"

/////////////////////////
// sprite
//
#include "nodes/sprite_nodes/Sprite.h"


/////////////////////////
// scene
//
#include "nodes/scene/Scene.h"


/////////////////////////
// root
//
#include "Director.h"


/////////////////////////
// opengl
//
#include "shaders/ShaderCache.h"




NS_CC_BEGIN

CC_DLL const char* cocos2dVersion();

NS_CC_END