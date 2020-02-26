#include "Director.h"
#include "Configuration.h"
#include "cocoa/AutoReleasePool.h"
#include "platform/PlatformBase.h"
#include "EGLView.h"
#include "shaders/GLStateCache.h"


NS_CC_BEGIN


static CCDirector* s_SharedDirector = NULL;
#define kDefaultFPS        60  // 60 frames per second


CCDirector* CCDirector::sharedDirector(void)
{
	if (!s_SharedDirector)
	{
		s_SharedDirector = new CCDirector();
		s_SharedDirector->init();
	}

	return s_SharedDirector;
}

long CCDirector::getClassTypeId()
{
	static const long id = getHashCodeByString(typeid(cocos2d::CCDirector).name());
	return id;
}

CCDirector::CCDirector()
	: _pobOpenGLView(NULL)
	, _dAnimationInterval(60.0)
	, _dOldAnimationInterval(60.0)
	, _bDisplayStats(false)
	, _eProjection(kCCDirectorProjectionDefault)
{
}

bool CCDirector::init(void)
{
	setDefaultValues();

	_pobOpenGLView = NULL;

	
	PoolManager::sharedPoolManager()->push();

	return true;
}

CCDirector::~CCDirector(void)
{
	// pop the autorelease pool
	PoolManager::sharedPoolManager()->pop();
	PoolManager::purgePoolManager();

	s_SharedDirector = NULL;
}


void CCDirector::setOpenGLView(EGLView* pobOpenGLView)
{
	CCAssert(pobOpenGLView, "opengl view should not be null");

	if (_pobOpenGLView != pobOpenGLView)
	{
		Configuration* conf = Configuration::sharedConfiguration();
		conf->gatherGPUInfo();
		conf->dumpInfo();

		// EAGLView is not a CCObject
		CC_SAFE_DELETE(_pobOpenGLView);
		_pobOpenGLView = pobOpenGLView;

		_obWinSizeInPoints = _pobOpenGLView->getDesignResolutionSize();

		if (_pobOpenGLView)
		{
			setGLDefaultValues();
		}

		CHECK_GL_ERROR_DEBUG();
	}
}


void CCDirector::setDefaultValues(void)
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


void CCDirector::setGLDefaultValues(void)
{
	CCAssert(_pobOpenGLView, "opengl view should not be null");

	setAlphaBlending(true);
	setDepthTest(false);
	setProjection(_eProjection);

	glClearColor(0.3f, 0.0f, 0.0f, 1.0f);
}


void CCDirector::setAlphaBlending(bool bOn)
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

void CCDirector::setDepthTest(bool bOn)
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


void CCDirector::setProjection(ccDirectorProjection kProjection)
{
	Size size = _obWinSizeInPoints;

	setViewport();

	switch (kProjection)
	{
	case kCCDirectorProjection2D:
	{
//		kmGLMatrixMode(KM_GL_PROJECTION);
//		kmGLLoadIdentity();
//#if CC_TARGET_PLATFORM == CC_PLATFORM_WP8
//		kmGLMultMatrix(CCEGLView::sharedOpenGLView()->getOrientationMatrix());
//#endif
//		kmMat4 orthoMatrix;
//		kmMat4OrthographicProjection(&orthoMatrix, 0, size.width, 0, size.height, -1024, 1024);
//		kmGLMultMatrix(&orthoMatrix);
//		kmGLMatrixMode(KM_GL_MODELVIEW);
//		kmGLLoadIdentity();
	}
	break;

	case kCCDirectorProjection3D:
	{
//		float zeye = this->getZEye();
//
//		kmMat4 matrixPerspective, matrixLookup;
//
//		kmGLMatrixMode(KM_GL_PROJECTION);
//		kmGLLoadIdentity();
//
//#if CC_TARGET_PLATFORM == CC_PLATFORM_WP8
//		//if needed, we need to add a rotation for Landscape orientations on Windows Phone 8 since it is always in Portrait Mode
//		kmGLMultMatrix(CCEGLView::sharedOpenGLView()->getOrientationMatrix());
//#endif
//		// issue #1334
//		kmMat4PerspectiveProjection(&matrixPerspective, 60, (GLfloat)size.width / size.height, 0.1f, zeye * 2);
//		// kmMat4PerspectiveProjection( &matrixPerspective, 60, (GLfloat)size.width/size.height, 0.1f, 1500);
//
//		kmGLMultMatrix(&matrixPerspective);
//
//		kmGLMatrixMode(KM_GL_MODELVIEW);
//		kmGLLoadIdentity();
//		kmVec3 eye, center, up;
//		kmVec3Fill(&eye, size.width / 2, size.height / 2, zeye);
//		kmVec3Fill(&center, size.width / 2, size.height / 2, 0.0f);
//		kmVec3Fill(&up, 0.0f, 1.0f, 0.0f);
//		kmMat4LookAt(&matrixLookup, &eye, &center, &up);
//		kmGLMultMatrix(&matrixLookup);
	}
	break;

	default:
		CCLOG("cocos2d: Director: unrecognized projection");
		break;
	}

	_eProjection = kProjection;
	//ccSetProjectionMatrixDirty();
}

void CCDirector::setViewport()
{
	if (_pobOpenGLView)
	{
		_pobOpenGLView->setViewPortInPoints(0, 0, _obWinSizeInPoints.width, _obWinSizeInPoints.height);
	}
}





NS_CC_END
