#include "EGLViewProtocol.h"
#include "platform/PlatformBase.h"
#include "GL/glew.h"
#include "touch/TouchDispatcher.h"
#include "cocoa/Dictionary.h"
#include "cocoa/Integer.h"
#include "touch/Touch.h"




NS_CC_BEGIN


static Touch* s_pTouches[CC_MAX_TOUCHES] = { NULL };
static unsigned int s_indexBitsUsed = 0;
static Dictionary s_TouchesIntergerDict;


// 取0位索引 表示没被占用
static int getUnUsedIndex()
{
	int i;
	int temp = s_indexBitsUsed;

	for (i = 0; i < CC_MAX_TOUCHES; i++) {
		if (!(temp & 0x00000001)) {
			s_indexBitsUsed |= (1 << i);
			return i;
		}

		temp >>= 1;
	}

	// all bits are used
	return -1;
}

static void removeUsedIndexBit(int index)
{
	if (index < 0 || index >= CC_MAX_TOUCHES)
		return;

	unsigned int temp = 1 << index;
	temp = ~temp;
	s_indexBitsUsed &= temp;
}





EGLViewProtocol::EGLViewProtocol()
	: _fScaleX(1.0f)
	, _fScaleY(1.0f)
	, _eResolutionPolicy(kResolutionShowAll)
	, _pTouchDelegate(NULL)
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

//////////////////////////////////////
// touch
//
void EGLViewProtocol::setTouchDelegate(EGLTouchDelegate* pDelegate)
{
	_pTouchDelegate = pDelegate;
}



void EGLViewProtocol::handleTouchesBegin(int num, int ids[], float xs[], float ys[])
{
	Set set;
	for (int i = 0; i < num; ++i)
	{
		int id = ids[i];
		float x = xs[i];
		float y = ys[i];

		Integer* pIndex = (Integer*)s_TouchesIntergerDict.objectForKey(id);
		int nUnusedIndex = 0;

		// it is a new touch
		if (pIndex == NULL)
		{
			nUnusedIndex = getUnUsedIndex();

			// The touches is more than MAX_TOUCHES ?
			if (nUnusedIndex == -1) {
				CCLOG("The touches is more than MAX_TOUCHES, nUnusedIndex = %d", nUnusedIndex);
				continue;
			}

			Touch* pTouch = s_pTouches[nUnusedIndex] = new Touch();
			//屏幕坐标先转为viewport坐标  再除_fScaleX后 转为designsize下的坐标
			pTouch->setTouchInfo(nUnusedIndex, (x - _rectViewPort.origin.x) / _fScaleX,
				(y - _rectViewPort.origin.y) / _fScaleY);

			//CCLOG("x = %f y = %f", pTouch->getLocationInView().x, pTouch->getLocationInView().y);

			Integer* pInterObj = new Integer(nUnusedIndex);
			s_TouchesIntergerDict.setObject(pInterObj, id);
			set.addObject(pTouch);
			pInterObj->release();
		}
	}

	if (set.count() == 0)
	{
		CCLOG("touchesBegan: count = 0");
		return;
	}

	_pTouchDelegate->touchesBegan(&set, NULL);
}

void EGLViewProtocol::handleTouchesMove(int num, int ids[], float xs[], float ys[])
{
	Set set;
	for (int i = 0; i < num; ++i)
	{
		int id = ids[i];
		float x = xs[i];
		float y = ys[i];

		Integer* pIndex = (Integer*)s_TouchesIntergerDict.objectForKey(id);
		if (pIndex == NULL) {
			CCLOG("if the index doesn't exist, it is an error");
			continue;
		}

		CCLOGINFO("Moving touches with id: %d, x=%f, y=%f", id, x, y);
		Touch* pTouch = s_pTouches[pIndex->getValue()];
		if (pTouch)
		{
			pTouch->setTouchInfo(pIndex->getValue(), (x - _rectViewPort.origin.x) / _fScaleX,
				(y - _rectViewPort.origin.y) / _fScaleY);

			set.addObject(pTouch);
		}
		else
		{
			// It is error, should return.
			CCLOG("Moving touches with id: %d error", id);
			return;
		}
	}

	if (set.count() == 0)
	{
		CCLOG("touchesMoved: count = 0");
		return;
	}

	_pTouchDelegate->touchesMoved(&set, NULL);
}

void EGLViewProtocol::handleTouchesEnd(int num, int ids[], float xs[], float ys[])
{
	Set set;
	getSetOfTouchesEndOrCancel(set, num, ids, xs, ys);
	_pTouchDelegate->touchesEnded(&set, NULL);
}

void EGLViewProtocol::handleTouchesCancel(int num, int ids[], float xs[], float ys[])
{
	Set set;
	getSetOfTouchesEndOrCancel(set, num, ids, xs, ys);
	_pTouchDelegate->touchesCancelled(&set, NULL);
}


void EGLViewProtocol::getSetOfTouchesEndOrCancel(Set& set, int num, int ids[], float xs[], float ys[])
{
	for (int i = 0; i < num; ++i)
	{
		int id = ids[i];
		float x = xs[i];
		float y = ys[i];

		Integer* pIndex = (Integer*)s_TouchesIntergerDict.objectForKey(id);
		if (pIndex == NULL)
		{
			CCLOG("if the index doesn't exist, it is an error");
			continue;
		}
		/* Add to the set to send to the director */
		Touch* pTouch = s_pTouches[pIndex->getValue()];
		if (pTouch)
		{
			CCLOGINFO("Ending touches with id: %d, x=%f, y=%f", id, x, y);
			pTouch->setTouchInfo(pIndex->getValue(), (x - _rectViewPort.origin.x) / _fScaleX,
				(y - _rectViewPort.origin.y) / _fScaleY);

			set.addObject(pTouch);

			// release the object
			pTouch->release();
			s_pTouches[pIndex->getValue()] = NULL;
			removeUsedIndexBit(pIndex->getValue());

			s_TouchesIntergerDict.removeObjectForKey(id);
		}
		else
		{
			CCLOG("Ending touches with id: %d error", id);
			return;
		}
	}

	if (set.count() == 0)
	{
		CCLOG("touchesEnded or touchesCancel: count = 0");
		return;
	}
}



NS_CC_END







