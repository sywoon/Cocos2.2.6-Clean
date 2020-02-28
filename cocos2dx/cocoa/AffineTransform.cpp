#include "AffineTransform.h"



NS_CC_BEGIN

AffineTransform __AffineTransformMake(float a, float b, float c, float d, float tx, float ty)
{
	AffineTransform t;
	t.a = a; t.b = b; t.c = c; t.d = d; t.tx = tx; t.ty = ty;
	return t;
}

/*
		|a  b  0|
(x,y,1)	|c  d  0|
		|tx ty 1|
*/
Point __PointApplyAffineTransform(const Point& point, const AffineTransform& t)
{
	Point p;
	p.x = (float)((double)t.a * point.x + (double)t.c * point.y + t.tx);
	p.y = (float)((double)t.b * point.x + (double)t.d * point.y + t.ty);
	return p;
}

// 对大小只做线性变换 不做平移变换
Size __SizeApplyAffineTransform(const Size& size, const AffineTransform& t)
{
	Size s;
	s.width = (float)((double)t.a * size.width + (double)t.c * size.height);
	s.height = (float)((double)t.b * size.width + (double)t.d * size.height);
	return s;
}


AffineTransform AffineTransformMakeIdentity()
{
	return __AffineTransformMake(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
}

extern const AffineTransform CCAffineTransformIdentity = AffineTransformMakeIdentity();


// 对四个顶点做变换  得到的新点根据大小 重新得到矩形
// 用于求包围盒
Rect RectApplyAffineTransform(const Rect& rect, const AffineTransform& anAffineTransform)
{
	float top = rect.getMinY();
	float left = rect.getMinX();
	float right = rect.getMaxX();
	float bottom = rect.getMaxY();

	Point topLeft = PointApplyAffineTransform(PointMake(left, top), anAffineTransform);
	Point topRight = PointApplyAffineTransform(PointMake(right, top), anAffineTransform);
	Point bottomLeft = PointApplyAffineTransform(PointMake(left, bottom), anAffineTransform);
	Point bottomRight = PointApplyAffineTransform(PointMake(right, bottom), anAffineTransform);

	float minX = min(min(topLeft.x, topRight.x), min(bottomLeft.x, bottomRight.x));
	float maxX = max(max(topLeft.x, topRight.x), max(bottomLeft.x, bottomRight.x));
	float minY = min(min(topLeft.y, topRight.y), min(bottomLeft.y, bottomRight.y));
	float maxY = max(max(topLeft.y, topRight.y), max(bottomLeft.y, bottomRight.y));

	return RectMake(minX, minY, (maxX - minX), (maxY - minY));
}



/* 平移矩阵 左乘目标矩阵
|1  0  0| |a  b  0|
|0  1  0| |c  d  0|
|tx ty 1| |tx ty 1|
*/
AffineTransform AffineTransformTranslate(const AffineTransform& t, float tx, float ty)
{
    return __AffineTransformMake(t.a, t.b, t.c, t.d, t.tx + t.a * tx + t.c * ty, t.ty + t.b * tx + t.d * ty);
}

/* 缩放矩阵 左乘目标矩阵
|sx 0  0| |a  b  0|
|0  sy 0| |c  d  0|
|0  0  1| |tx ty 1|
*/
AffineTransform AffineTransformScale(const AffineTransform& t, float sx, float sy)
{
    return __AffineTransformMake(t.a * sx, t.b * sx, t.c * sy, t.d * sy, t.tx, t.ty);
}

/* 旋转矩阵 左乘目标矩阵
|cosa  sina 0| |a  b  0|
|-sina cosa 0| |c  d  0|
|  0    0   1| |tx ty 1|
*/
AffineTransform AffineTransformRotate(const AffineTransform& t, float anAngle)
{
    float fSin = sin(anAngle);
    float fCos = cos(anAngle);

    return __AffineTransformMake(t.a * fCos + t.c * fSin,
        t.b * fCos + t.d * fSin,
        t.c * fCos - t.a * fSin,
        t.d * fCos - t.b * fSin,
        t.tx,
        t.ty);
}

/* Concatenate `t2' to `t1' and return the result:
     t' = t1 * t2 */
/* 矩阵 t1左乘t2
|a  b  0| |a  b  0|
|c  d  0| |c  d  0|
|tx ty 1| |tx ty 1|
*/
AffineTransform AffineTransformConcat(const AffineTransform& t1, const AffineTransform& t2)
{
    return __AffineTransformMake(t1.a * t2.a + t1.b * t2.c, t1.a * t2.b + t1.b * t2.d, //a,b
        t1.c * t2.a + t1.d * t2.c, t1.c * t2.b + t1.d * t2.d, //c,d
        t1.tx * t2.a + t1.ty * t2.c + t2.tx,                  //tx
        t1.tx * t2.b + t1.ty * t2.d + t2.ty);                  //ty
}

/* Return true if `t1' and `t2' are equal, false otherwise. */
// 浮点数用强==？
bool AffineTransformEqualToTransform(const AffineTransform& t1, const AffineTransform& t2)
{
    //return (t1.a == t2.a && t1.b == t2.b && t1.c == t2.c && t1.d == t2.d && t1.tx == t2.tx && t1.ty == t2.ty);

    return (fabsf(t1.a - t2.a) < FLT_EPSILON)
        && (fabsf(t1.b - t2.b) < FLT_EPSILON)
        && (fabsf(t1.c - t2.c) < FLT_EPSILON)
        && (fabsf(t1.d - t2.d) < FLT_EPSILON)
        && (fabsf(t1.tx - t2.tx) < FLT_EPSILON)
        && (fabsf(t1.ty - t2.ty) < FLT_EPSILON);
}

AffineTransform AffineTransformInvert(const AffineTransform& t)
{
    float determinant = 1 / (t.a * t.d - t.b * t.c);

    return __AffineTransformMake(determinant * t.d, -determinant * t.b, -determinant * t.c, determinant * t.a,
        determinant * (t.c * t.ty - t.d * t.tx), determinant * (t.b * t.tx - t.a * t.ty));
}




NS_CC_END