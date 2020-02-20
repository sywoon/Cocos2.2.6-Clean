#pragma once

#include "Object.h"
#include <math.h>

NS_CC_BEGIN


class CC_DLL Size;

class CC_DLL Point
{
public:
	float x;
	float y;

public:
	Point();
	Point(float x, float y);
	Point(const Point& other);
	Point(const Size& size);

	Point&	operator= (const Point& other);
	Point&	operator= (const Size& size);
	Point	operator+(const Point& right) const;
	Point	operator-(const Point& right) const;
	Point	operator-() const;
	Point	operator*(float a) const;
	Point	operator/(float a) const;


public:
	void	setPoint(float x, float y);

	bool	equals(const Point& target) const;


	inline float getLength() const {
		return sqrtf(x * x + y * y);
	};

	inline float getLengthSq() const {
		return dot(*this); //x*x + y*y;
	};

	inline float getDistanceSq(const Point & other) const {
		return (*this - other).getLengthSq();
	};

	inline float getDistance(const Point & other) const {
		return (*this - other).getLength();
	};

	inline float getAngle() const {
		return atan2f(y, x);
	};

	float getAngle(const Point & other) const;


	inline float dot(const Point & other) const {
		return x * other.x + y * other.y;
	};

	inline float cross(const Point & other) const {
		return x * other.y - y * other.x;
	};

	// Calculates perpendicular of v, rotated 90 degrees counter-clockwise -- cross(v, perp(v)) >= 0
	inline Point getPerp() const {
		return Point(-y, x);
	};

	// Calculates perpendicular of v, rotated 90 degrees clockwise -- cross(v, rperp(v)) <= 0
	inline Point getRPerp() const {
		return Point(y, -x);
	};

	// Calculates the projection of this over other.
	inline Point project(const Point & other) const {
		return other * (dot(other) / other.dot(other));
	};

	//Complex multiplication of two points ("rotates" two points).
	// @return CCPoint vector with an angle of this.getAngle() + other.getAngle(),
	// and a length of this.getLength() * other.getLength().
	inline Point rotate(const Point & other) const {
		return Point(x * other.x - y * other.y, x * other.y + y * other.x);
	};

	/** Unrotates two points.
	 @return CCPoint vector with an angle of this.getAngle() - other.getAngle(),
	 and a length of this.getLength() * other.getLength().
	 */
	inline Point unrotate(const Point & other) const {
		return Point(x * other.x + y * other.y, y * other.x - x * other.y);
	};

	inline Point normalize() const {
		float length = getLength();
		if (length == 0.) return Point(1.f, 0);
		return *this / getLength();
	};

	/** Linear Interpolation between two points a and b
		alpha == 0 ? a
		alpha == 1 ? b
		otherwise a value between a..b
	 */
	inline Point lerp(const Point & other, float alpha) const {
		return *this* (1.f - alpha) + other * alpha;
	};

	/** Rotates a point counter clockwise by the angle around a pivot
	 @param pivot is the pivot, naturally
	 @param angle is the angle of rotation ccw in radians
	 @returns the rotated point
	 */
	Point rotateByAngle(const Point & pivot, float angle) const;

	static inline Point forAngle(const float a)
	{
		return Point(cosf(a), sinf(a));
	}
};


class CC_DLL Size
{
public:
	float width;
	float height;

public:
	Size();
	Size(float width, float height);
	Size(const Size& other);
	Size(const Point& point);

	Size& operator= (const Size& other);
	Size& operator= (const Point& point);
	Size operator+(const Size& right) const;
	Size operator-(const Size& right) const;
	Size operator*(float a) const;
	Size operator/(float a) const;

	void setSize(float width, float height);
	bool equals(const Size& target) const;
};


class CC_DLL Rect
{
public:
	Point origin;
	Size  size;

public:
	Rect();
	Rect(float x, float y, float width, float height);
	Rect(const Rect& other);

	Rect& operator= (const Rect& other);

	void  setRect(float x, float y, float width, float height);
	float getMinX() const; /// return the leftmost x-value of current rect
	float getMidX() const; /// return the midpoint x-value of current rect
	float getMaxX() const; /// return the rightmost x-value of current rect
	float getMinY() const; /// return the bottommost y-value of current rect
	float getMidY() const; /// return the midpoint y-value of current rect
	float getMaxY() const; /// return the topmost y-value of current rect

	bool equals(const Rect& rect) const;
	bool containsPoint(const Point& point) const;
	bool intersectsRect(const Rect& rect) const;
};

NS_CC_END