#include "PointExtend.h"


NS_CC_BEGIN

#define kPointEpsilon FLT_EPSILON

float ccpLength(const Point& v)
{
	return v.getLength();
}

float ccpDistance(const Point& v1, const Point& v2)
{
	return (v1 - v2).getLength();
}

Point ccpNormalize(const Point& v)
{
	return v.normalize();
}

Point ccpForAngle(const float a)
{
	return Point::forAngle(a);
}

float ccpToAngle(const Point& v)
{
	return v.getAngle();
}

Point ccpLerp(const Point& a, const Point& b, float alpha)
{
	return a.lerp(b, alpha);
}

float clampf(float value, float min_inclusive, float max_inclusive)
{
	if (min_inclusive > max_inclusive) {
		CC_SWAP(min_inclusive, max_inclusive, float);
	}
	return value < min_inclusive ? min_inclusive : value < max_inclusive ? value : max_inclusive;
}

Point ccpClamp(const Point& p, const Point& min_inclusive, const Point& max_inclusive)
{
	return ccp(clampf(p.x, min_inclusive.x, max_inclusive.x), clampf(p.y, min_inclusive.y, max_inclusive.y));
}

Point ccpFromSize(const Size& s)
{
	return Point(s);
}

Point ccpCompOp(const Point& p, float (*opFunc)(float))
{
	return ccp(opFunc(p.x), opFunc(p.y));
}

Point ccpCompMult(const Point& a, const Point& b)
{
	return ccp(a.x * b.x, a.y * b.y);
}

float ccpAngleSigned(const Point& a, const Point& b)
{
	return a.getAngle(b);
}

Point ccpRotateByAngle(const Point& v, const Point& pivot, float angle)
{
	return v.rotateByAngle(pivot, angle);
}


bool ccpSegmentIntersect(const Point& A, const Point& B, const Point& C, const Point& D)
{
	float S, T;

	if (ccpLineIntersect(A, B, C, D, &S, &T)
		&& (S >= 0.0f && S <= 1.0f && T >= 0.0f && T <= 1.0f))
		return true;

	return false;
}

Point ccpIntersectPoint(const Point& A, const Point& B, const Point& C, const Point& D)
{
	float S, T;

	if (ccpLineIntersect(A, B, C, D, &S, &T))
	{
		// Point of intersection
		Point P;
		P.x = A.x + S * (B.x - A.x);
		P.y = A.y + S * (B.y - A.y);
		return P;
	}

	return PointZero;
}

bool ccpLineIntersect(const Point& A, const Point& B,
	const Point& C, const Point& D,
	float* S, float* T)
{
	// FAIL: Line undefined
	if ((A.x == B.x && A.y == B.y) || (C.x == D.x && C.y == D.y))
	{
		return false;
	}
	const float BAx = B.x - A.x;
	const float BAy = B.y - A.y;
	const float DCx = D.x - C.x;
	const float DCy = D.y - C.y;
	const float ACx = A.x - C.x;
	const float ACy = A.y - C.y;

	const float denom = DCy * BAx - DCx * BAy;

	*S = DCx * ACy - DCy * ACx;
	*T = BAx * ACy - BAy * ACx;

	if (denom == 0)
	{
		if (*S == 0 || *T == 0)
		{
			// Lines incident
			return true;
		}
		// Lines parallel and not incident
		return false;
	}

	*S = *S / denom;
	*T = *T / denom;

	// Point of intersection
	// CGPoint P;
	// P.x = A.x + *S * (B.x - A.x);
	// P.y = A.y + *S * (B.y - A.y);

	return true;
}

float ccpAngle(const Point& a, const Point& b)
{
	float angle = acosf(ccpDot(ccpNormalize(a), ccpNormalize(b)));
	if (fabs(angle) < kPointEpsilon) return 0.f;
	return angle;
}

NS_CC_END

