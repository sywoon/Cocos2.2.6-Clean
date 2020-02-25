#include "EGLViewProtocol.h"
#include "platform/PlatformBase.h"
#include "GL/glew.h"


NS_CC_BEGIN



EGLViewProtocol::EGLViewProtocol()
	: _fScaleX(1.0f)
	, _fScaleY(1.0f)
	, _eResolutionPolicy(kResolutionUnKnown)
{
}

EGLViewProtocol::~EGLViewProtocol()
{
}


void EGLViewProtocol::setFrameSize(float width, float height)
{
	_sizeFrame = SizeMake(width, height);
	_sizeDesignResolution = _sizeFrame;
}

const Size& EGLViewProtocol::getFrameSize() const
{
	return _sizeFrame;
}

void EGLViewProtocol::setDesignResolutionSize(float width, float height, ResolutionPolicy resolutionPolicy)
{
	CCAssert(resolutionPolicy != kResolutionUnKnown, "should set resolutionPolicy");

	if (width == 0.0f || height == 0.0f)
		return;

	_sizeDesignResolution.setSize(width, height);

	_fScaleX = (float)_sizeFrame.width / _sizeDesignResolution.width;
	_fScaleY = (float)_sizeFrame.height / _sizeDesignResolution.height;

	if (resolutionPolicy == kResolutionNoBorder)
	{
		_fScaleX = _fScaleY = MAX(_fScaleX, _fScaleY);
	}

	if (resolutionPolicy == kResolutionShowAll)
	{
		_fScaleX = _fScaleY = MIN(_fScaleX, _fScaleY);
	}

	if (resolutionPolicy == kResolutionFixedHeight) {
		_fScaleX = _fScaleY;
		_sizeDesignResolution.width = ceilf(_sizeFrame.width / _fScaleX);
	}

	if (resolutionPolicy == kResolutionFixedWidth) {
		_fScaleY = _fScaleX;
		_sizeDesignResolution.height = ceilf(_sizeFrame.height / _fScaleY);
	}

	// 根据适配策略  计算出viewport  目标：怎样尽量填满窗口
	float viewPortW = _sizeDesignResolution.width * _fScaleX;
	float viewPortH = _sizeDesignResolution.height * _fScaleY;

	//注意：(0,0)点可能在外部
	_rectViewPort.setRect((_sizeFrame.width - viewPortW) / 2, (_sizeFrame.height - viewPortH) / 2, viewPortW, viewPortH);

	_eResolutionPolicy = resolutionPolicy;
}

const Size & EGLViewProtocol::getDesignResolutionSize() const
{
	return _sizeDesignResolution;
}


Size EGLViewProtocol::getVisibleSize() const
{
	if (_eResolutionPolicy == kResolutionNoBorder || _eResolutionPolicy == kResolutionShowAll)
	{
		return SizeMake(_sizeFrame.width / _fScaleX, _sizeFrame.height / _fScaleY);
	}
	else
	{
		return _sizeDesignResolution;
	}
}

Point EGLViewProtocol::getVisibleOrigin() const
{
	if (_eResolutionPolicy == kResolutionNoBorder || _eResolutionPolicy == kResolutionShowAll)
	{
		return PointMake((_sizeDesignResolution.width - _sizeFrame.width / _fScaleX) / 2,
			(_sizeDesignResolution.height - _sizeFrame.height / _fScaleY) / 2);
	}
	else
	{
		return PointZero;
	}
}


void EGLViewProtocol::setViewPortInPoints(float x, float y, float w, float h)
{
	const float xx = x * _fScaleX + _rectViewPort.origin.x;
	const float yy = y * _fScaleY + _rectViewPort.origin.y;
	const float width = w * _fScaleX;
	const float height = h * _fScaleY;

	_rectViewPort.setRect(xx, yy, width, height);

	glViewport((GLint)xx,
		(GLint)yy,
		(GLsizei)width,
		(GLsizei)height);
}

void EGLViewProtocol::setScissorInPoints(float x, float y, float w, float h)
{
	glScissor((GLint)(x * _fScaleX + _rectViewPort.origin.x),
		(GLint)(y * _fScaleY + _rectViewPort.origin.y),
		(GLsizei)(w * _fScaleX),
		(GLsizei)(h * _fScaleY));
}

bool EGLViewProtocol::isScissorEnabled()
{
	return (GL_FALSE == glIsEnabled(GL_SCISSOR_TEST)) ? false : true;
}

const Rect& EGLViewProtocol::getViewPortRect() const
{
	return _rectViewPort;
}

Rect EGLViewProtocol::getScissorRect()
{
	GLfloat params[4];
	glGetFloatv(GL_SCISSOR_BOX, params);
	float x = (params[0] - _rectViewPort.origin.x) / _fScaleX;
	float y = (params[1] - _rectViewPort.origin.y) / _fScaleY;
	float w = params[2] / _fScaleX;
	float h = params[3] / _fScaleY;
	return RectMake(x, y, w, h);
}

void EGLViewProtocol::setViewName(const char* pszViewName)
{
	if (pszViewName != NULL && strlen(pszViewName) > 0)
	{
		strncpy(_szViewName, pszViewName, sizeof(_szViewName));
	}
}

const char* EGLViewProtocol::getViewName()
{
	return _szViewName;
}

float EGLViewProtocol::getScaleX() const
{
	return _fScaleX;
}

float EGLViewProtocol::getScaleY() const
{
	return _fScaleY;
}



NS_CC_END







