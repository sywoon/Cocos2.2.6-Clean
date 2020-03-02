#include "ShaderCache.h"
#include "GLProgram.h"
#include "cocoa/Dictionary.h"
#include "ShaderFiles.h"



NS_CC_BEGIN

static ShaderCache* _sharedShaderCache = 0;

ShaderCache* ShaderCache::sharedShaderCache()
{
	if (!_sharedShaderCache) {
		_sharedShaderCache = new ShaderCache();
		if (!_sharedShaderCache->init())
		{
			CC_SAFE_DELETE(_sharedShaderCache);
		}
	}
	return _sharedShaderCache;
}

void ShaderCache::purgeSharedShaderCache()
{
	CC_SAFE_RELEASE_NULL(_sharedShaderCache);
}



ShaderCache::ShaderCache()
	: m_pPrograms(0)
{
}

ShaderCache::~ShaderCache()
{
	CCLOGINFO("cocos2d deallocing 0x%X", this);
	m_pPrograms->release();
}

bool ShaderCache::init()
{
	m_pPrograms = new Dictionary();
	loadDefaultShaders();
	return true;
}

GLProgram* ShaderCache::programForKey(const char* key)
{
	return (GLProgram*)m_pPrograms->objectForKey(key);
}

void ShaderCache::addProgram(GLProgram* program, const char* key)
{
	m_pPrograms->setObject(program, key);
}



void ShaderCache::loadDefaultShaders()
{
	// Position Texture Color shader
	GLProgram* p = new GLProgram();
	loadDefaultShader(p, kCCShaderType_PositionTextureColor);

	m_pPrograms->setObject(p, kCCShader_PositionTextureColor);
	p->release();

	// Position Texture Color alpha test
	p = new GLProgram();
	loadDefaultShader(p, kCCShaderType_PositionTextureColorAlphaTest);

	m_pPrograms->setObject(p, kCCShader_PositionTextureColorAlphaTest);
	p->release();

	// Position, Color shader
	p = new GLProgram();
	loadDefaultShader(p, kCCShaderType_PositionColor);

	m_pPrograms->setObject(p, kCCShader_PositionColor);
	p->release();

	// Position Texture shader
	p = new GLProgram();
	loadDefaultShader(p, kCCShaderType_PositionTexture);

	m_pPrograms->setObject(p, kCCShader_PositionTexture);
	p->release();

	// Position, Texture attribs, 1 Color as uniform shader
	p = new GLProgram();
	loadDefaultShader(p, kCCShaderType_PositionTexture_uColor);

	m_pPrograms->setObject(p, kCCShader_PositionTexture_uColor);
	p->release();

	// Position Texture A8 Color shader
	p = new GLProgram();
	loadDefaultShader(p, kCCShaderType_PositionTextureA8Color);

	m_pPrograms->setObject(p, kCCShader_PositionTextureA8Color);
	p->release();

	// Position and 1 color passed as a uniform (to simulate glColor4ub )
	p = new GLProgram();
	loadDefaultShader(p, kCCShaderType_Position_uColor);

	m_pPrograms->setObject(p, kCCShader_Position_uColor);
	p->release();

	// Position, Legth(TexCoords, Color (used by Draw Node basically )
	p = new GLProgram();
	loadDefaultShader(p, kCCShaderType_PositionLengthTexureColor);

	m_pPrograms->setObject(p, kCCShader_PositionLengthTexureColor);
	p->release();

	// ControlSwitch
	p = new GLProgram();
	loadDefaultShader(p, kCCShaderType_ControlSwitch);

	m_pPrograms->setObject(p, kCCShader_ControlSwitch);
	p->release();
}

void ShaderCache::reloadDefaultShaders()
{
	// reset all programs and reload them

	// Position Texture Color shader
	GLProgram* p = programForKey(kCCShader_PositionTextureColor);
	p->reset();
	loadDefaultShader(p, kCCShaderType_PositionTextureColor);

	// Position Texture Color alpha test
	p = programForKey(kCCShader_PositionTextureColorAlphaTest);
	p->reset();
	loadDefaultShader(p, kCCShaderType_PositionTextureColorAlphaTest);

	// Position, Color shader
	p = programForKey(kCCShader_PositionColor);
	p->reset();
	loadDefaultShader(p, kCCShaderType_PositionColor);

	// Position Texture shader
	p = programForKey(kCCShader_PositionTexture);
	p->reset();
	loadDefaultShader(p, kCCShaderType_PositionTexture);

	// Position, Texture attribs, 1 Color as uniform shader
	p = programForKey(kCCShader_PositionTexture_uColor);
	p->reset();
	loadDefaultShader(p, kCCShaderType_PositionTexture_uColor);

	// Position Texture A8 Color shader
	p = programForKey(kCCShader_PositionTextureA8Color);
	p->reset();
	loadDefaultShader(p, kCCShaderType_PositionTextureA8Color);

	// Position and 1 color passed as a uniform (to simulate glColor4ub )
	p = programForKey(kCCShader_Position_uColor);
	p->reset();
	loadDefaultShader(p, kCCShaderType_Position_uColor);

	// Position, Legth(TexCoords, Color (used by Draw Node basically )
	p = programForKey(kCCShader_PositionLengthTexureColor);
	p->reset();
	loadDefaultShader(p, kCCShaderType_PositionLengthTexureColor);
}


void ShaderCache::loadDefaultShader(GLProgram* p, int type)
{
	switch (type) {
	case kCCShaderType_PositionTextureColor:
		p->initWithVertexShaderByteArray(ccPositionTextureColor_vert, ccPositionTextureColor_frag);

		p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
		p->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
		p->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

		break;
	case kCCShaderType_PositionTextureColorAlphaTest:
		p->initWithVertexShaderByteArray(ccPositionTextureColor_vert, ccPositionTextureColorAlphaTest_frag);

		p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
		p->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
		p->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

		break;
	case kCCShaderType_PositionColor:
		p->initWithVertexShaderByteArray(ccPositionColor_vert, ccPositionColor_frag);

		p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
		p->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);

		break;
	case kCCShaderType_PositionTexture:
		p->initWithVertexShaderByteArray(ccPositionTexture_vert, ccPositionTexture_frag);

		p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
		p->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

		break;
	case kCCShaderType_PositionTexture_uColor:
		p->initWithVertexShaderByteArray(ccPositionTexture_uColor_vert, ccPositionTexture_uColor_frag);

		p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
		p->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

		break;
	case kCCShaderType_PositionTextureA8Color:
		p->initWithVertexShaderByteArray(ccPositionTextureA8Color_vert, ccPositionTextureA8Color_frag);

		p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
		p->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
		p->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

		break;
	case kCCShaderType_Position_uColor:
		p->initWithVertexShaderByteArray(ccPosition_uColor_vert, ccPosition_uColor_frag);

		p->addAttribute("aVertex", kCCVertexAttrib_Position);

		break;
	case kCCShaderType_PositionLengthTexureColor:
		p->initWithVertexShaderByteArray(ccPositionColorLengthTexture_vert, ccPositionColorLengthTexture_frag);

		p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
		p->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
		p->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);

		break;

	case kCCShaderType_ControlSwitch:
		p->initWithVertexShaderByteArray(ccPositionTextureColor_vert, ccExSwitchMask_frag);

		p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
		p->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
		p->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

		break;

	default:
		CCLOG("cocos2d: %s:%d, error shader type", __FUNCTION__, __LINE__);
		return;
	}

	p->link();
	p->updateUniforms();

	CHECK_GL_ERROR_DEBUG();
}




NS_CC_END