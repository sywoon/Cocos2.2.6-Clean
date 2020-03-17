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


class EGLTouchDelegate;
class CC_DLL EGLViewProtocol
{
public:
	EGLViewProtocol();
	virtual ~EGLViewProtocol();

public:
	virtual void    end() = 0;
	virtual bool    isOpenGLReady() = 0;
	virtual void    swapBuffers() = 0;

public:
	virtual void		setFrameSize(float width, float height);
	virtual const Size& getFrameSize() const;

	virtual void		setDesignResolutionSize(float width, float height, ResolutionPolicy resolutionPolicy);
	virtual const Size& getDesignResolutionSize() const;

	// 设计分辨率下的大小
	virtual Size		getVisibleSize() const;
	virtual Point		getVisibleOrigin() const;

	virtual bool		isScissorEnabled();

	// 设计分辨率下的大小  原点基于_rectViewPort.origin
	virtual void		setViewPortInPoints(float x, float y, float w, float h);
	virtual void		setScissorInPoints(float x, float y, float w, float h);

	const Rect&			getViewPortRect() const;
	virtual Rect		getScissorRect();

	virtual void		setViewName(const char* pszViewName);
	const char*			getViewName();

	float				getScaleX() const;
	float				getScaleY() const;

public:
	virtual void		setTouchDelegate(EGLTouchDelegate* pDelegate);

	virtual void		handleTouchesBegin(int num, int ids[], float xs[], float ys[]);
	virtual void		handleTouchesMove(int num, int ids[], float xs[], float ys[]);
	virtual void		handleTouchesEnd(int num, int ids[], float xs[], float ys[]);
	virtual void		handleTouchesCancel(int num, int ids[], float xs[], float ys[]);

private:
	void				getSetOfTouchesEndOrCancel(Set& set, int num, int ids[], float xs[], float ys[]);



protected:
	// real screen size
	Size		_sizeFrame;
	// resolution size, it is the size appropriate for the app resources.
	Size		_sizeDesignResolution;
	Rect		_rectViewPort;

	char		_szViewName[50];

	// framesize / desiginsize
	float		_fScaleX;
	float		_fScaleY;
	ResolutionPolicy _eResolutionPolicy;

	EGLTouchDelegate* _pTouchDelegate;
};


NS_CC_END