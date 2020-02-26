#pragma once

#include "ccMacros.h"
#include "cocoa/Object.h"
#include "ccTypeInfo.h"
#include "ccClassMacros.h"
#include "cocoa/Geometry.h"


NS_CC_BEGIN

typedef enum {
	/// sets a 2D projection (orthogonal projection)
	kCCDirectorProjection2D,

	/// sets a 3D projection with a fovy=60, znear=0.5f and zfar=1500.
	kCCDirectorProjection3D,

	/// Default projection is 3D projection
	kCCDirectorProjectionDefault = kCCDirectorProjection3D,
} ccDirectorProjection;


class EGLView;
class CC_DLL CCDirector : public Object, public TypeInfo
{
public:
	CCDirector();
	virtual ~CCDirector(void);

	static CCDirector*	sharedDirector(void);


public:
	virtual long		getClassTypeId();

	inline EGLView*		getOpenGLView(void) { return _pobOpenGLView; }
	void				setOpenGLView(EGLView* pobOpenGLView);


protected:
	bool				init(void);
	void				setDefaultValues(void);
	void				setGLDefaultValues(void);

	void				setAlphaBlending(bool bOn);
	void				setDepthTest(bool bOn);

	void				setProjection(ccDirectorProjection kProjection);
	void				setViewport();


protected:
	EGLView*			_pobOpenGLView;
	ccDirectorProjection _eProjection;

	double				_dAnimationInterval;
	double				_dOldAnimationInterval;
	bool				_bDisplayStats;

	Size				_obWinSizeInPoints;

	// CCEGLViewProtocol will recreate stats labels to fit visible rect
	friend class CCEGLViewProtocol;
};



NS_CC_END

