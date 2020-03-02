#include "Director.h"
#include "Configuration.h"
#include "cocoa/AutoReleasePool.h"
#include "cocoa/extend/PointExtend.h"
#include "platform/PlatformBase.h"
#include "EGLView.h"
#include "shaders/GLStateCache.h"
#include "kazmath/GL/matrix.h"

#include "keypad/KeypadDispatcher.h"
#include "touch/TouchDispatcher.h"
#include "Accelerometer.h"


unsigned int g_uNumberOfDraws = 0;


NS_CC_BEGIN


static Director* s_SharedDirector = NULL;
#define kDefaultFPS        60  // 60 frames per second


Director* Director::sharedDirector(void)
{
	if (!s_SharedDirector)
	{
		s_SharedDirector = new Director();
		s_SharedDirector->init();
	}

	return s_SharedDirector;
}

long Director::getClassTypeId()
{
	static const long id = getHashCodeByString(typeid(cocos2d::Director).name());
	return id;
}

Director::Director()
	: _pobOpenGLView(NULL)
	, _dAnimationInterval(1/60.0)
	, _dOldAnimationInterval(1/60.0)
	, _bDisplayStats(false)
	, _eProjection(kCCDirectorProjectionDefault)

	, _uTotalFrames(0)
	, _uFrames(0)
	, _fSecondsPerFrame(0.0f)
	, _fDeltaTime(0.0f)
	, _pLastUpdate(NULL)

	, _pKeypadDispatcher(NULL)
	, _pTouchDispatcher(NULL)
	, _pAccelerometer(NULL)
{
}

bool Director::init(void)
{
	setDefaultValues();

	_pobOpenGLView = NULL;
	_pLastUpdate = new struct cc_timeval();

	_pKeypadDispatcher = new KeypadDispatcher();
	_pTouchDispatcher = new TouchDispatcher();
	_pAccelerometer = new Accelerometer();
	
	PoolManager::sharedPoolManager()->push();

	return true;
}

Director::~Director(void)
{
	CC_SAFE_RELEASE(_pKeypadDispatcher);
	CC_SAFE_RELEASE(_pTouchDispatcher);
	CC_SAFE_DELETE(_pAccelerometer);


	PoolManager::sharedPoolManager()->pop();
	PoolManager::purgePoolManager();

	CC_SAFE_DELETE(_pLastUpdate);

	s_SharedDirector = NULL;
}

void Director::purgeDirector()
{
	_pTouchDispatcher->removeAllDelegates();

	stopAnimation();

	CHECK_GL_ERROR_DEBUG();

	// OpenGL view
	_pobOpenGLView->end();
	_pobOpenGLView = NULL;

	// delete this
	release();
}


void Director::stopAnimation(void)
{
	_bDrawScene = true;
}

void Director::startAnimation(void)
{
	_bDrawScene = false;
}

void Director::setOpenGLView(EGLView* pobOpenGLView)
{
	CCAssert(pobOpenGLView, "opengl view should not be null");

	if (_pobOpenGLView == pobOpenGLView)
		return;

	Configuration* conf = Configuration::sharedConfiguration();
	conf->gatherGPUInfo();
	conf->dumpInfo();

	// EAGLView is not a CCObject
	CC_SAFE_DELETE(_pobOpenGLView);
	_pobOpenGLView = pobOpenGLView;

	_sizeWinInPoints = _pobOpenGLView->getDesignResolutionSize();

	if (_pobOpenGLView)
	{
		setGLDefaultValues();
	}

	CHECK_GL_ERROR_DEBUG();

	_pobOpenGLView->setTouchDelegate(_pTouchDispatcher);
	_pTouchDispatcher->setDispatchEvents(true);
}


void Director::setDefaultValues(void)
{
	Configuration* conf = Configuration::sharedConfiguration();

	double fps = conf->getNumber("cocos2d.x.fps", kDefaultFPS);
	_dOldAnimationInterval = _dAnimationInterval = 1.0 / fps;

	_bDisplayStats = conf->getBool("cocos2d.x.display_fps", false);

	const char* projection = conf->getCString("cocos2d.x.gl.projection", "3d");
	if (strcmp(projection, "3d") == 0)
		_eProjection = kCCDirectorProjection3D;
	else if (strcmp(projection, "2d") == 0)
		_eProjection = kCCDirectorProjection2D;
	else
		CCAssert(false, "Invalid projection value");
}


void Director::setGLDefaultValues(void)
{
	CCAssert(_pobOpenGLView, "opengl view should not be null");

	setAlphaBlending(true);
	setDepthTest(false);
	setProjection(_eProjection);

	glClearColor(0.3f, 0.0f, 0.0f, 1.0f);
}


void Director::setAlphaBlending(bool bOn)
{
	if (bOn)
	{
		ccGLBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
	}
	else
	{
		ccGLBlendFunc(GL_ONE, GL_ZERO);
	}

	CHECK_GL_ERROR_DEBUG();
}

void Director::setDepthTest(bool bOn)
{
	if (bOn)
	{
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   //?
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
	CHECK_GL_ERROR_DEBUG();
}


void Director::setProjection(ccDirectorProjection kProjection)
{
	Size size = _sizeWinInPoints;

	setViewport();

	switch (kProjection)
	{
	case kCCDirectorProjection2D:
	{
		kmGLMatrixMode(KM_GL_PROJECTION);
		kmGLLoadIdentity();

		kmMat4 orthoMatrix;
		kmMat4OrthographicProjection(&orthoMatrix, 0, size.width, 0, size.height, -1024, 1024);
		kmGLMultMatrix(&orthoMatrix);

		kmGLMatrixMode(KM_GL_MODELVIEW);
		kmGLLoadIdentity();
	}
	break;

	case kCCDirectorProjection3D:
	{
		float zeye = this->getZEye();

		kmMat4 matrixPerspective, matrixLookup;

		kmGLMatrixMode(KM_GL_PROJECTION);
		kmGLLoadIdentity();

		kmMat4PerspectiveProjection(&matrixPerspective, 60, (GLfloat)size.width / size.height, 0.1f, zeye * 2);

		kmGLMultMatrix(&matrixPerspective);

		kmGLMatrixMode(KM_GL_MODELVIEW);
		kmGLLoadIdentity();
		kmVec3 eye, center, up;
		kmVec3Fill(&eye, size.width / 2, size.height / 2, zeye);
		kmVec3Fill(&center, size.width / 2, size.height / 2, 0.0f);
		kmVec3Fill(&up, 0.0f, 1.0f, 0.0f);
		kmMat4LookAt(&matrixLookup, &eye, &center, &up);
		kmGLMultMatrix(&matrixLookup);
	}
	break;

	default:
		CCLOG("cocos2d: Director: unrecognized projection");
		break;
	}

	_eProjection = kProjection;
}

float Director::getZEye(void)
{
	return (_sizeWinInPoints.height / 1.1566f);
}

void Director::setViewport()
{
	if (_pobOpenGLView)
	{
		_pobOpenGLView->setViewPortInPoints(0, 0, _sizeWinInPoints.width, _sizeWinInPoints.height);
	}
}

void Director::end()
{
	_bPurgeDirecotorInNextLoop = true;
}


void Director::mainLoop(void)
{
	if (_bPurgeDirecotorInNextLoop)
	{
		_bPurgeDirecotorInNextLoop = false;
		purgeDirector();
	}
	else if (!_bDrawScene)
	{
		drawScene();

		PoolManager::sharedPoolManager()->pop();
	}
}

void Director::drawScene(void)
{
	calculateDeltaTime();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	kmGLPushMatrix();

	//if (m_pRunningScene)
	//{
	//	m_pRunningScene->visit();
	//}

	kmGLPopMatrix();

	_uTotalFrames++;

	if (_pobOpenGLView)
	{
		_pobOpenGLView->swapBuffers();
	}
}


void Director::calculateDeltaTime(void)
{
	struct cc_timeval now;

	if (CCTime::gettimeofdayCocos2d(&now, NULL) != 0)
	{
		CCLOG("error in gettimeofday");
		_fDeltaTime = 0;
		return;
	}

	_fDeltaTime = (now.tv_sec - _pLastUpdate->tv_sec) + (now.tv_usec - _pLastUpdate->tv_usec) / 1000000.0f;
	_fDeltaTime = MAX(0, _fDeltaTime);

#ifdef DEBUG   //vs调试时为true
	// If we are debugging our code, prevent big delta time
	if (_fDeltaTime > 0.2f)
	{
		_fDeltaTime = 1 / 60.0f;
	}
#endif

	*_pLastUpdate = now;
}

void Director::setAnimationInterval(double dValue)
{
	_dAnimationInterval = dValue;
	if (!_bDrawScene)
	{
		stopAnimation();
		startAnimation();
	}
}


Size Director::getWinSize(void)
{
	return _sizeWinInPoints;
}



static void GLToClipTransform(kmMat4* transformOut)
{
	kmMat4 projection;
	kmGLGetMatrix(KM_GL_PROJECTION, &projection);

	kmMat4 modelview;
	kmGLGetMatrix(KM_GL_MODELVIEW, &modelview);

	kmMat4Multiply(transformOut, &projection, &modelview);
}

// 屏幕下的触摸坐标 转为 设计坐标下的 opengl世界坐标(Model为单位矩阵)/当前模型下的坐标
Point Director::convertToGL(const Point& uiPoint)
{
	kmMat4 transform;
	GLToClipTransform(&transform);  // mvp矩阵

	kmMat4 transformInv;
	kmMat4Inverse(&transformInv, &transform);

	// Calculate z=0 using -> transform*[0, 0, 0, 1]/w
	kmScalar zClip = transform.mat[14] / transform.mat[15];

	Size glSize = _pobOpenGLView->getDesignResolutionSize();
	// 屏幕坐标 转到[-1,1]
	kmVec3 clipCoord = { 2.0f * uiPoint.x / glSize.width - 1.0f, 1.0f - 2.0f * uiPoint.y / glSize.height, zClip };

	kmVec3 glCoord;
	kmVec3TransformCoord(&glCoord, &clipCoord, &transformInv);

	return ccp(glCoord.x, glCoord.y);
}

// 世界坐标下的点(Model为单位矩阵)/模型下的点 转 ui坐标
Point Director::convertToUI(const Point& glPoint)
{
	kmMat4 transform;
	GLToClipTransform(&transform);

	kmVec3 clipCoord;
	// Need to calculate the zero depth from the transform.
	kmVec3 glCoord = { glPoint.x, glPoint.y, 0.0 };
	kmVec3TransformCoord(&clipCoord, &glCoord, &transform);

	Size glSize = _pobOpenGLView->getDesignResolutionSize();
	return ccp(glSize.width * (clipCoord.x * 0.5 + 0.5), glSize.height * (-clipCoord.y * 0.5 + 0.5));
}


//////////////////////////////////////
// Keypad Touch Accelerometer
//
void Director::setKeypadDispatcher(KeypadDispatcher* pKeypadDispatcher)
{
	CC_SAFE_RETAIN(pKeypadDispatcher);
	CC_SAFE_RELEASE(_pKeypadDispatcher);
	_pKeypadDispatcher = pKeypadDispatcher;
}

KeypadDispatcher* Director::getKeypadDispatcher()
{
	return _pKeypadDispatcher;
}

void Director::setTouchDispatcher(TouchDispatcher* pTouchDispatcher)
{
	if (_pTouchDispatcher != pTouchDispatcher)
	{
		CC_SAFE_RETAIN(pTouchDispatcher);
		CC_SAFE_RELEASE(_pTouchDispatcher);
		_pTouchDispatcher = pTouchDispatcher;
	}
}

TouchDispatcher* Director::getTouchDispatcher()
{
	return _pTouchDispatcher;
}

void Director::setAccelerometer(Accelerometer* pAccelerometer)
{
	if (_pAccelerometer != pAccelerometer)
	{
		CC_SAFE_DELETE(_pAccelerometer);
		_pAccelerometer = pAccelerometer;
	}
}

Accelerometer* Director::getAccelerometer()
{
	return _pAccelerometer;
}



NS_CC_END
