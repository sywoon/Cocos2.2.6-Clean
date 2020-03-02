#pragma once

#include "ccMacros.h"
#include "LayerColor.h"


NS_CC_BEGIN


class CC_DLL LayerGradient : public LayerColor
{
public:
	static LayerGradient* create();
	static LayerGradient* create(const ccColor4B& start, const ccColor4B& end);
	static LayerGradient* create(const ccColor4B& start, const ccColor4B& end, const Point& v);

protected:
	virtual bool	init();
	virtual bool	initWithColor(const ccColor4B& start, const ccColor4B& end);
	virtual bool	initWithColor(const ccColor4B& start, const ccColor4B& end, const Point& v);

protected:
	virtual void	updateColor();

public:
	virtual void	setCompressedInterpolation(bool bCompressedInterpolation);
	virtual bool	isCompressedInterpolation();


protected:
	// Whether or not the interpolation will be compressed in order to display all the colors 
	// of the gradient both in canonical and non canonical vectors
	// Default: YES
	// —πÀı≤Â÷µ
	bool m_bCompressedInterpolation;

protected:
	CC_PROPERTY_PASS_BY_REF(ccColor3B, _startColor, StartColor)
	CC_PROPERTY_PASS_BY_REF(ccColor3B, _endColor, EndColor)
	CC_PROPERTY(GLubyte, _cStartOpacity, StartOpacity)
	CC_PROPERTY(GLubyte, _cEndOpacity, EndOpacity)
	CC_PROPERTY_PASS_BY_REF(Point, _alongVector, Vector)
};



NS_CC_END