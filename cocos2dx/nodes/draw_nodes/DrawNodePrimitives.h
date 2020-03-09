#pragma once

#include "ccMacros.h"


NS_CC_BEGIN

class PointArray;


void CC_DLL ccDrawInit();
void CC_DLL ccDrawFree();

void CC_DLL ccDrawColor4B(GLubyte r, GLubyte g, GLubyte b, GLubyte a);
void CC_DLL ccDrawColor4F(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void CC_DLL ccPointSize(GLfloat pointSize);
void CC_DLL ccLineWidth(GLfloat width);


void CC_DLL ccDrawPoint(const Point& point);
void CC_DLL ccDrawPoints(const Point* points, unsigned int numberOfPoints);

void CC_DLL ccDrawLine(const Point& origin, const Point& destination);
void CC_DLL ccDrawRect(Point origin, Point destination);
void CC_DLL ccDrawSolidRect(Point origin, Point destination, ccColor4F color);

// The polygon can be closed or open
void CC_DLL ccDrawPoly(const Point* vertices, unsigned int numOfVertices, bool closePolygon);
void CC_DLL ccDrawSolidPoly(const Point* poli, unsigned int numberOfPoints, ccColor4F color);

void CC_DLL ccDrawCircle(const Point& center, float radius, float angle, unsigned int segments, bool drawLineToCenter, float scaleX, float scaleY);
void CC_DLL ccDrawCircle(const Point& center, float radius, float angle, unsigned int segments, bool drawLineToCenter);

// @warning This function could be pretty slow. Use it only for debugging purposes.
void CC_DLL ccDrawQuadBezier(const Point& origin, const Point& control, const Point& destination, unsigned int segments);

// @warning This function could be pretty slow. Use it only for debugging purposes.
void CC_DLL ccDrawCubicBezier(const Point& origin, const Point& control1, const Point& control2, const Point& destination, unsigned int segments);

/*
// @warning This function could be pretty slow. Use it only for debugging purposes.
void CC_DLL ccDrawCatmullRom(PointArray* arrayOfControlPoints, unsigned int segments);

// @warning This function could be pretty slow. Use it only for debugging purposes.
void CC_DLL ccDrawCardinalSpline(PointArray* config, float tension, unsigned int segments);
*/



NS_CC_END
