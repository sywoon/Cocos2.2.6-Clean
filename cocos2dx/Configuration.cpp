#include "Configuration.h"
#include "cocoa/String.h"
#include "cocoa/Bool.h"
#include "cocoa/Integer.h"
#include "cocoa/Double.h"
#include "cocos2d.h"
#include "platform/PlatformMacros.h"
#include <string>


using namespace std;

NS_CC_BEGIN


Configuration* Configuration::s_gSharedConfiguration = NULL;


Configuration* Configuration::sharedConfiguration(void)
{
	if (!s_gSharedConfiguration)
	{
		s_gSharedConfiguration = new Configuration();
		s_gSharedConfiguration->init();
	}

	return s_gSharedConfiguration;
}

void Configuration::purgeConfiguration(void)
{
	CC_SAFE_RELEASE_NULL(s_gSharedConfiguration);
}



Configuration::Configuration(void)
	: _nMaxTextureSize(0)
	, _bSupportsPVRTC(false)
	, _bSupportsNPOT(false)
	, _bSupportsBGRA8888(false)
	, _bSupportsDiscardFramebuffer(false)
	, _bSupportsShareableVAO(false)
	, _nMaxSamplesAllowed(0)
	, _nMaxTextureUnits(0)
	, _pGlExtensions(NULL)
	, _pValueDict(NULL)
{
}

bool Configuration::init(void)
{
	_pValueDict = Dictionary::create();
	_pValueDict->retain();

	_pValueDict->setObject(String::create(cocos2dVersion()), "cocos2d.x.version");


#if CC_ENABLE_PROFILERS
	_pValueDict->setObject(Bool::create(true), "cocos2d.x.compiled_with_profiler");
#else
	_pValueDict->setObject(Bool::create(false), "cocos2d.x.compiled_with_profiler");
#endif

#if CC_ENABLE_GL_STATE_CACHE == 0
	_pValueDict->setObject(Bool::create(false), "cocos2d.x.compiled_with_gl_state_cache");
#else
	_pValueDict->setObject(CCBool::create(true), "cocos2d.x.compiled_with_gl_state_cache");
#endif

	return true;
}

Configuration::~Configuration(void)
{
	_pValueDict->release();
}

void Configuration::loadConfigFile(const char* filename)
{
	//todo
}


void Configuration::gatherGPUInfo()
{
	_pValueDict->setObject(String::create((const char*)glGetString(GL_VENDOR)), "gl.vendor");
	_pValueDict->setObject(String::create((const char*)glGetString(GL_RENDERER)), "gl.renderer");
	_pValueDict->setObject(String::create((const char*)glGetString(GL_VERSION)), "gl.version");

	_pGlExtensions = (char*)glGetString(GL_EXTENSIONS);

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_nMaxTextureSize);
	_pValueDict->setObject(Integer::create((int)_nMaxTextureSize), "gl.max_texture_size");

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_nMaxTextureUnits);
	_pValueDict->setObject(Integer::create((int)_nMaxTextureUnits), "gl.max_texture_units");

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	glGetIntegerv(GL_MAX_SAMPLES_APPLE, &_nMaxSamplesAllowed);
	_pValueDict->setObject(Integer::create((int)_nMaxSamplesAllowed), "gl.max_samples_allowed");
#endif

	_bSupportsPVRTC = checkForGLExtension("GL_IMG_texture_compression_pvrtc");
	_pValueDict->setObject(Bool::create(_bSupportsPVRTC), "gl.supports_PVRTC");

	_bSupportsNPOT = true;
	_pValueDict->setObject(Bool::create(_bSupportsNPOT), "gl.supports_NPOT");

	_bSupportsBGRA8888 = checkForGLExtension("GL_IMG_texture_format_BGRA888");
	_pValueDict->setObject(Bool::create(_bSupportsBGRA8888), "gl.supports_BGRA8888");

	_bSupportsDiscardFramebuffer = checkForGLExtension("GL_EXT_discard_framebuffer");
	_pValueDict->setObject(Bool::create(_bSupportsDiscardFramebuffer), "gl.supports_discard_framebuffer");

	_bSupportsShareableVAO = checkForGLExtension("vertex_array_object");
	_pValueDict->setObject(Bool::create(_bSupportsShareableVAO), "gl.supports_vertex_array_object");

	CHECK_GL_ERROR_DEBUG();
}

int Configuration::getMaxTextureSize(void) const
{
	return _nMaxTextureSize;
}

int Configuration::getMaxTextureUnits(void) const
{
	return _nMaxTextureUnits;
}

bool Configuration::supportsNPOT(void) const
{
	return _bSupportsNPOT;
}

bool Configuration::supportsPVRTC(void) const
{
	return _bSupportsPVRTC;
}

bool Configuration::supportsBGRA8888(void) const
{
	return _bSupportsBGRA8888;
}

bool Configuration::supportsDiscardFramebuffer(void) const
{
	return _bSupportsDiscardFramebuffer;
}

bool Configuration::supportsShareableVAO(void) const
{
	return _bSupportsShareableVAO;
}


bool Configuration::checkForGLExtension(const string& searchName) const
{
	bool bRet = false;
	const char* kSearchName = searchName.c_str();

	if (_pGlExtensions &&
		strstr(_pGlExtensions, kSearchName))
	{
		bRet = true;
	}

	return bRet;
}


const char* Configuration::getCString(const char* key, const char* default_value) const
{
	Object* ret = _pValueDict->objectForKey(key);
	if (ret) {
		if (String* str = dynamic_cast<String*>(ret))
			return str->getCString();

		CCAssert(false, "Key found, but from different type");
	}

	return default_value;
}

bool Configuration::getBool(const char* key, bool default_value) const
{
	Object* ret = _pValueDict->objectForKey(key);
	if (ret) {
		if (Bool * boolobj = dynamic_cast<Bool*>(ret))
			return boolobj->getValue();
		if (String * strobj = dynamic_cast<String*>(ret))
			return strobj->boolValue();
		CCAssert(false, "Key found, but from different type");
	}

	return default_value;
}

double Configuration::getNumber(const char* key, double default_value) const
{
	Object* ret = _pValueDict->objectForKey(key);
	if (ret) {
		if (Double * obj = dynamic_cast<Double*>(ret))
			return obj->getValue();

		if (Integer * obj = dynamic_cast<Integer*>(ret))
			return obj->getValue();

		if (String * strobj = dynamic_cast<String*>(ret))
			return strobj->doubleValue();

		CCAssert(false, "Key found, but from different type");
	}

	return default_value;
}

Object * Configuration::getObject(const char* key) const
{
	return _pValueDict->objectForKey(key);
}

void Configuration::setObject(const char* key, Object * value)
{
	_pValueDict->setObject(value, key);
}



void Configuration::dumpInfo(void) const
{
	// Dump
	PrettyPrinter visitor(0);
	_pValueDict->acceptVisitor(visitor);

	CCLOG("%s", visitor.getResult().c_str());


	// And Dump some warnings as well
#if CC_ENABLE_PROFILERS
	CCLOG("cocos2d: **** WARNING **** CC_ENABLE_PROFILERS is defined. Disable it when you finish profiling (from ccConfig.h)");
	printf("\n");
#endif

#if CC_ENABLE_GL_STATE_CACHE == 0
	//CCLOG("");
	//CCLOG("cocos2d: **** WARNING **** CC_ENABLE_GL_STATE_CACHE is disabled. To improve performance, enable it (from ccConfig.h)");
	printf("\n");
#endif

}




NS_CC_END
