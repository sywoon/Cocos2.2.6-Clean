#pragma once

#include "ccMacros.h"


NS_CC_BEGIN

enum {
	kCCShaderType_PositionTextureColor,
	kCCShaderType_PositionTextureColorAlphaTest,
	kCCShaderType_PositionColor,
	kCCShaderType_PositionTexture,
	kCCShaderType_PositionTexture_uColor,
	kCCShaderType_PositionTextureA8Color,
	kCCShaderType_Position_uColor,
	kCCShaderType_PositionLengthTexureColor,
	kCCShaderType_ControlSwitch,

	kCCShaderType_MAX,
};

#define kCCShader_PositionTextureColor              "ShaderPositionTextureColor"
#define kCCShader_PositionTextureColorAlphaTest     "ShaderPositionTextureColorAlphaTest"
#define kCCShader_PositionColor                     "ShaderPositionColor"
#define kCCShader_PositionTexture                   "ShaderPositionTexture"
#define kCCShader_PositionTexture_uColor            "ShaderPositionTexture_uColor"
#define kCCShader_PositionTextureA8Color            "ShaderPositionTextureA8Color"
#define kCCShader_Position_uColor                   "ShaderPosition_uColor"
#define kCCShader_PositionLengthTexureColor         "ShaderPositionLengthTextureColor"
#define kCCShader_ControlSwitch                     "Shader_ControlSwitch"



class GLProgram;
class Dictionary;
class CC_DLL ShaderCache : public Object
{
public:
    ShaderCache();
    virtual ~ShaderCache();

    static ShaderCache*		sharedShaderCache();
    static void				purgeSharedShaderCache();

public:
    void		loadDefaultShaders();
    void		reloadDefaultShaders();

    void		addProgram(GLProgram* program, const char* key);
    GLProgram*	programForKey(const char* key);


private:
	bool		init();
	void		loadDefaultShader(GLProgram* program, int type);


private:
	Dictionary* m_pPrograms;
};


NS_CC_END