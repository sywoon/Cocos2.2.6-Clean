#pragma once
#include "ccMacros.h"
#include "Geometry.h"


NS_CC_BEGIN

/*
|a  b  0|
|c  d  0|
|tx ty 1|
*/
struct AffineTransform {
	float a, b, c, d;
	float tx, ty;
};

extern CC_DLL const AffineTransform CCAffineTransformIdentity;

// 创建一个仿射矩阵
CC_DLL AffineTransform __AffineTransformMake(float a, float b, float c, float d, float tx, float ty);
#define AffineTransformMake __AffineTransformMake

// 点仿射变换
CC_DLL Point __PointApplyAffineTransform(const Point& point, const AffineTransform& t);
#define PointApplyAffineTransform __PointApplyAffineTransform

// 大小线性变换 少了平移变换
CC_DLL Size __SizeApplyAffineTransform(const Size& size, const AffineTransform& t);
#define SizeApplyAffineTransform __SizeApplyAffineTransform

// 单位矩阵
CC_DLL AffineTransform AffineTransformMakeIdentity();
CC_DLL Rect RectApplyAffineTransform(const Rect& rect, const AffineTransform& anAffineTransform);


CC_DLL AffineTransform AffineTransformTranslate(const AffineTransform& t, float tx, float ty);
CC_DLL AffineTransform AffineTransformRotate(const AffineTransform& aTransform, float anAngle);
CC_DLL AffineTransform AffineTransformScale(const AffineTransform& t, float sx, float sy);
CC_DLL AffineTransform AffineTransformConcat(const AffineTransform& t1, const AffineTransform& t2);
CC_DLL bool AffineTransformEqualToTransform(const AffineTransform& t1, const AffineTransform& t2);
CC_DLL AffineTransform AffineTransformInvert(const AffineTransform& t);



NS_CC_END
