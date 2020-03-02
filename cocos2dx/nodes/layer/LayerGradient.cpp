#include "LayerGradient.h"
#include "cocoa/Geometry.h"


NS_CC_BEGIN

LayerGradient* LayerGradient::create()
{
	LayerGradient* pRet = new LayerGradient();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}

LayerGradient* LayerGradient::create(const ccColor4B& start, const ccColor4B& end)
{
	LayerGradient* pLayer = new LayerGradient();
	if (pLayer && pLayer->initWithColor(start, end))
	{
		pLayer->autorelease();
		return pLayer;
	}
	CC_SAFE_DELETE(pLayer);
	return NULL;
}

LayerGradient* LayerGradient::create(const ccColor4B& start, const ccColor4B& end, const Point& v)
{
	LayerGradient* pLayer = new LayerGradient();
	if (pLayer && pLayer->initWithColor(start, end, v))
	{
		pLayer->autorelease();
		return pLayer;
	}
	CC_SAFE_DELETE(pLayer);
	return NULL;
}



bool LayerGradient::init()
{
	return initWithColor(ccc4(0, 0, 0, 255), ccc4(0, 0, 0, 255));
}

bool LayerGradient::initWithColor(const ccColor4B& start, const ccColor4B& end)
{
	return initWithColor(start, end, ccp(0, -1));
}

bool LayerGradient::initWithColor(const ccColor4B& start, const ccColor4B& end, const Point& v)
{
	_endColor.r = end.r;
	_endColor.g = end.g;
	_endColor.b = end.b;

	_cEndOpacity = end.a;
	_cStartOpacity = start.a;
	_alongVector = v;

	m_bCompressedInterpolation = true;

	return LayerColor::initWithColor(ccc4(start.r, start.g, start.b, 255));
}

void LayerGradient::updateColor()
{
	LayerColor::updateColor();

	float h = ccpLength(_alongVector);
	if (h == 0)
		return;

	float c = sqrtf(2.0f);
	Point u = ccp(_alongVector.x / h, _alongVector.y / h);

	// Compressed Interpolation mode
	if (m_bCompressedInterpolation)
	{
		float h2 = 1 / (fabsf(u.x) + fabsf(u.y));
		u = ccpMult(u, h2 * (float)c);
	}

	float opacityf = (float)_displayedOpacity / 255.0f;

	ccColor4F S = {
		_displayedColor.r / 255.0f,
		_displayedColor.g / 255.0f,
		_displayedColor.b / 255.0f,
		_cStartOpacity * opacityf / 255.0f
	};

	ccColor4F E = {
		_endColor.r / 255.0f,
		_endColor.g / 255.0f,
		_endColor.b / 255.0f,
		_cEndOpacity * opacityf / 255.0f
	};

	// (-1, -1)
	_pSquareColors[0].r = E.r + (S.r - E.r) * ((c + u.x + u.y) / (2.0f * c));
	_pSquareColors[0].g = E.g + (S.g - E.g) * ((c + u.x + u.y) / (2.0f * c));
	_pSquareColors[0].b = E.b + (S.b - E.b) * ((c + u.x + u.y) / (2.0f * c));
	_pSquareColors[0].a = E.a + (S.a - E.a) * ((c + u.x + u.y) / (2.0f * c));
	// (1, -1)
	_pSquareColors[1].r = E.r + (S.r - E.r) * ((c - u.x + u.y) / (2.0f * c));
	_pSquareColors[1].g = E.g + (S.g - E.g) * ((c - u.x + u.y) / (2.0f * c));
	_pSquareColors[1].b = E.b + (S.b - E.b) * ((c - u.x + u.y) / (2.0f * c));
	_pSquareColors[1].a = E.a + (S.a - E.a) * ((c - u.x + u.y) / (2.0f * c));
	// (-1, 1)
	_pSquareColors[2].r = E.r + (S.r - E.r) * ((c + u.x - u.y) / (2.0f * c));
	_pSquareColors[2].g = E.g + (S.g - E.g) * ((c + u.x - u.y) / (2.0f * c));
	_pSquareColors[2].b = E.b + (S.b - E.b) * ((c + u.x - u.y) / (2.0f * c));
	_pSquareColors[2].a = E.a + (S.a - E.a) * ((c + u.x - u.y) / (2.0f * c));
	// (1, 1)
	_pSquareColors[3].r = E.r + (S.r - E.r) * ((c - u.x - u.y) / (2.0f * c));
	_pSquareColors[3].g = E.g + (S.g - E.g) * ((c - u.x - u.y) / (2.0f * c));
	_pSquareColors[3].b = E.b + (S.b - E.b) * ((c - u.x - u.y) / (2.0f * c));
	_pSquareColors[3].a = E.a + (S.a - E.a) * ((c - u.x - u.y) / (2.0f * c));
}

bool LayerGradient::isCompressedInterpolation()
{
	return m_bCompressedInterpolation;
}

void LayerGradient::setCompressedInterpolation(bool compress)
{
	m_bCompressedInterpolation = compress;
	updateColor();
}





const ccColor3B& LayerGradient::getStartColor()
{
	return _realColor;
}

void LayerGradient::setStartColor(const ccColor3B& color)
{
	setColor(color);
}

void LayerGradient::setEndColor(const ccColor3B& color)
{
	_endColor = color;
	updateColor();
}

const ccColor3B& LayerGradient::getEndColor()
{
	return _endColor;
}

void LayerGradient::setStartOpacity(GLubyte o)
{
	_cStartOpacity = o;
	updateColor();
}

GLubyte LayerGradient::getStartOpacity()
{
	return _cStartOpacity;
}

void LayerGradient::setEndOpacity(GLubyte o)
{
	_cEndOpacity = o;
	updateColor();
}

GLubyte LayerGradient::getEndOpacity()
{
	return _cEndOpacity;
}

void LayerGradient::setVector(const Point& var)
{
	_alongVector = var;
	updateColor();
}

const Point& LayerGradient::getVector()
{
	return _alongVector;
}



NS_CC_END