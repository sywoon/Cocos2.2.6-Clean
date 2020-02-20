#pragma once

#include "ccMacros.h"
#include "cocoa/Geometry.h"

enum ResolutionPolicy
{
	kResolutionExactFit,
	kResolutionNoBorder,
	kResolutionShowAll,
	kResolutionFixedHeight,
	kResolutionFixedWidth,

	kResolutionUnKnown,
};

NS_CC_BEGIN


class CC_DLL CCEGLViewProtocol
{
//public:
//	CCEGLViewProtocol();
//	virtual ~CCEGLViewProtocol();
//
//
//protected:
//	// real screen size
//	Size m_obScreenSize;
//	// resolution size, it is the size appropriate for the app resources.
//	Size m_obDesignResolutionSize;
//	// the view port size
//	Rect m_obViewPortRect;
//
//	// the view name
//	char   m_szViewName[50];
//
//	float  m_fScaleX;
//	float  m_fScaleY;
//	ResolutionPolicy m_eResolutionPolicy;
};


NS_CC_END