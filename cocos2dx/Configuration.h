#pragma once

#include "ccMacros.h"
#include "cocoa/Object.h"
#include "cocoa/Dictionary.h"
#include "GL/glew.h"



NS_CC_BEGIN

class CC_DLL Configuration : public Object
{
public:
	virtual ~Configuration(void);

	static Configuration* sharedConfiguration(void);
	static void purgeConfiguration(void);

public:
	/** Loads a config file. If the keys are already present, then they are going to be replaced. Otherwise the new keys are added. */
	void		loadConfigFile(const char* filename);

	/** gathers OpenGL / GPU information  在opengl初始化后调用 */
	void		gatherGPUInfo(void);


public:
	int			getMaxTextureSize(void) const;
	int			getMaxTextureUnits(void) const;

	// GPU supports NPOT (Non Power Of Two) textures.
	bool		supportsNPOT(void) const;

	// PVR Texture Compressed
	bool		supportsPVRTC(void) const;
	bool		supportsBGRA8888(void) const;

	// glDiscardFramebufferEXT is supported
	bool		supportsDiscardFramebuffer(void) const;
	bool		supportsShareableVAO(void) const;

	bool		checkForGLExtension(const std::string& searchName) const;


public:
	const char* getCString(const char* key, const char* default_value = NULL) const;
	bool		getBool(const char* key, bool default_value = false) const;
	double		getNumber(const char* key, double default_value = 0.0) const;
	Object*		getObject(const char* key) const;
	void		setObject(const char* key, Object* value);

	void		dumpInfo(void) const;


private:
	Configuration(void);
	bool		init(void);


private:
	static Configuration*	s_gSharedConfiguration;
	static std::string		s_sConfigfile;
	Dictionary*				_pValueDict;

	char*			_pGlExtensions;
	GLint           _nMaxTextureSize;
	GLint           _nMaxTextureUnits;
	GLint           _nMaxSamplesAllowed;
	bool            _bSupportsPVRTC;
	bool            _bSupportsNPOT;
	bool            _bSupportsBGRA8888;
	bool            _bSupportsDiscardFramebuffer;
	bool            _bSupportsShareableVAO;
};





NS_CC_END