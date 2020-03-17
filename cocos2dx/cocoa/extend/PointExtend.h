#pragma once
#include "ccMacros.h"
#include "cocoa/Geometry.h"


NS_CC_BEGIN

#define ccp(__X__,__Y__) cocos2d::PointMake((float)(__X__), (float)(__Y__))

static inline Point ccpNeg(const Point& v)
{
    return -v;
}


static inline Point ccpAdd(const Point& v1, const Point& v2)
{
    return v1 + v2;
}


static inline Point ccpSub(const Point& v1, const Point& v2)
{
    return v1 - v2;
}

static inline Point ccpMult(const Point& v, const float s)
{
    return v * s;
}

static inline Point ccpMidpoint(const Point& v1, const Point& v2)
{
    return (v1 + v2) / 2.f;
}

static inline float ccpDot(const Point& v1, const Point& v2)
{
    return v1.dot(v2);
}

static inline float ccpCross(const Point& v1, const Point& v2)
{
    return v1.cross(v2);
}

// 垂直线 逆时针90度
static inline Point ccpPerp(const Point& v)
{
    return v.getPerp();
}

// 垂直线 顺时针90度
static inline Point ccpRPerp(const Point& v)
{
    return v.getRPerp();
}


// v1在v2的投影
static inline Point ccpProject(const Point& v1, const Point& v2)
{
    return v1.project(v2);
}

// 新向量 角度为两者之和 长度为两者之积
static inline Point ccpRotate(const Point& v1, const Point& v2)
{
    return v1.rotate(v2);
}

// 新向量 角度为两者之差 长度为两者之积
static inline Point ccpUnrotate(const Point& v1, const Point& v2)
{
    return v1.unrotate(v2);
}


static inline float ccpLengthSQ(const Point& v)
{
    return v.getLengthSq();
}


static inline float ccpDistanceSQ(const Point p1, const Point p2)
{
    return (p1 - p2).getLengthSq();
}


float CC_DLL ccpLength(const Point& v);

float CC_DLL ccpDistance(const Point& v1, const Point& v2);


Point CC_DLL ccpNormalize(const Point& v);

Point CC_DLL ccpForAngle(const float a);

float CC_DLL ccpToAngle(const Point& v);

float CC_DLL clampf(float value, float min_inclusive, float max_inclusive);

Point CC_DLL ccpClamp(const Point& p, const Point& from, const Point& to);

Point CC_DLL ccpFromSize(const Size& s);

Point CC_DLL ccpCompOp(const Point& p, float (*opFunc)(float));

Point CC_DLL ccpLerp(const Point& a, const Point& b, float alpha);

Point CC_DLL ccpCompMult(const Point& a, const Point& b);

float CC_DLL ccpAngleSigned(const Point& a, const Point& b);

float CC_DLL ccpAngle(const Point& a, const Point& b);

Point CC_DLL ccpRotateByAngle(const Point& v, const Point& pivot, float angle);

/** A general line-line intersection test
 @param p1
    is the startpoint for the first line P1 = (p1 - p2)
 @param p2
    is the endpoint for the first line P1 = (p1 - p2)
 @param p3
    is the startpoint for the second line P2 = (p3 - p4)
 @param p4
    is the endpoint for the second line P2 = (p3 - p4)
 @param s
    is the range for a hitpoint in P1 (pa = p1 + s*(p2 - p1))
 @param t
    is the range for a hitpoint in P3 (pa = p2 + t*(p4 - p3))
 @return bool
    indicating successful intersection of a line
    note that to truly test intersection for segments we have to make
    sure that s & t lie within [0..1] and for rays, make sure s & t > 0
    the hit point is        p3 + t * (p4 - p3);
    the hit point also is    p1 + s * (p2 - p1);
 @since v0.99.1
 */
bool CC_DLL ccpLineIntersect(const Point& p1, const Point& p2,
    const Point& p3, const Point& p4,
    float* s, float* t);

//returns YES if Segment A-B intersects with segment C-D
bool CC_DLL ccpSegmentIntersect(const Point& A, const Point& B, const Point& C, const Point& D);

// returns the intersection point of line A-B, C-D
Point CC_DLL ccpIntersectPoint(const Point& A, const Point& B, const Point& C, const Point& D);


NS_CC_END