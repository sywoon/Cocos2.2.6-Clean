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
class CC_DLL Director : public Object, public TypeInfo
{
public:
	Director();
	virtual ~Director(void);

	static Director*	sharedDirector(void);


public:
	virtual long		getClassTypeId();
	virtual void		mainLoop(void);
	virtual void		end(void);

	// 保持主逻辑运行 但不再渲染
	virtual void		startAnimation(void);
	virtual void		stopAnimation(void);
	

	inline EGLView*		getOpenGLView(void) { return _pobOpenGLView; }
	void				setOpenGLView(EGLView* pobOpenGLView);

	inline unsigned int getTotalFrames(void) { return _uTotalFrames; }
	inline double		getAnimationInterval(void) { return _dAnimationInterval; }
	virtual void		setAnimationInterval(double dValue);

protected:
	void				purgeDirector();

	bool				init(void);
	void				setDefaultValues(void);
	void				setGLDefaultValues(void);

	void				setAlphaBlending(bool bOn);
	void				setDepthTest(bool bOn);

	void				setProjection(ccDirectorProjection kProjection);
	void				setViewport();
	float				getZEye(void);

	void				drawScene(void);

	void				calculateDeltaTime();

protected:
	EGLView*			_pobOpenGLView;
	ccDirectorProjection _eProjection;

	double				_dAnimationInterval;
	double				_dOldAnimationInterval;
	bool				_bDisplayStats;

	Size				_sizeWinInPoints;

	bool				_bPurgeDirecotorInNextLoop;
	bool				_bDrawScene;

	unsigned int		_uTotalFrames;
	unsigned int		_uFrames;
	float				_fSecondsPerFrame;

	float				_fDeltaTime;
	struct cc_timeval*	_pLastUpdate;


	// CCEGLViewProtocol will recreate stats labels to fit visible rect
	friend class CCEGLViewProtocol;
};



NS_CC_END

